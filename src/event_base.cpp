#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "definition.h"
#include "event_base.h"
#include "handler/handler_listener.h"
#include "helper.h"

using namespace std;

static int s_epollFd;
time_t g_timeNow = time(NULL);
time_t g_timeLast = g_timeNow;

CEventBase* CEventBase::_instance = NULL;

CEventBase* CEventBase::instance()
{
	if(NULL == CEventBase::_instance)
		CEventBase::_instance = new CEventBase();
	return CEventBase::_instance;
}

void CEventBase::release()
{
	if(NULL != CEventBase::_instance){
		delete CEventBase::_instance;
		CEventBase::_instance = NULL;
	}
}

void CEventBase::start()
{
	if(EBS_STOPPED != this->_status)
		throw VAS_ERR_INTERNAL;
	
	this->_status = EBS_RUNNING;

	struct epoll_event events[64];
	while(EBS_RUNNING == this->_status){
		g_timeNow = time(NULL);
		if(g_timeNow >= (g_timeLast + 1)){
			this->_timer();
			g_timeLast = g_timeNow;
		}

		int nfds = epoll_wait(s_epollFd, events, 64, 1000);
		if (nfds == -1)
			continue;

		for(int i = 0; i < nfds; ++i){
			if(events[i].events & EPOLLIN){
				map<int, CHandler*>::iterator iter = this->_sockets.find(events[i].data.fd);
				CHandler *handler = (this->_sockets.end() == iter) ? NULL : iter->second;
				if((NULL != handler) && (NULL != handler->_input))
					if(!CHelper::Socket::read(handler->_fd, handler->_input))
						this->_close(events[i].data.fd, handler, VAS_REASON_SERVER_CLOSED);

				if(NULL != handler)
					handler->onRead();

				if(this->_jobs.size() > 0)
					this->commit();
			}
			if(events[i].events & EPOLLOUT){
				map<int, CHandler*>::iterator iter = this->_sockets.find(events[i].data.fd);
				CHandler *handler = (this->_sockets.end() == iter) ? NULL : iter->second;
				if((NULL != handler) && (NULL != handler->_output) && (handler->_output->size() > 0))
					if(!CHelper::Socket::write(handler->_fd, handler->_output))
						this->_close(events[i].data.fd, handler, VAS_REASON_SERVER_CLOSED);

				if(NULL != handler)
					handler->onWritten();
			}
        }
    }
	this->_status = EBS_STOPPED;
}

void CEventBase::stop()
{
	this->_status = EBS_STOPPING;
}

void CEventBase::add(const CEventBase::JOB& job)
{
	this->_jobs.push(job);
}

void CEventBase::commit()
{
	while(!this->_jobs.empty()){
		CEventBase::JOB job = this->_jobs.front();
		this->_jobs.pop();
		switch(job.cmd){
			case VAS_COMMAND_ADD_FD:
				this->_add(job.fd, job.payload.handler);
				break;
			case VAS_COMMAND_DEL_FD:
				this->_close(job.fd, job.payload.handler, VAS_REASON_SERVER_CLOSED);
				break;
			case VAS_COMMAND_ADD_BUFFER:
				this->_addBuffer(job.fd, job.payload.buffer);
				break;
		}
	}
}

/////////////////////////////////////////////////////////

CEventBase::CEventBase() : _status(EBS_STOPPED)
{
	s_epollFd = epoll_create(64);
	if(s_epollFd < 0)
		throw VAS_ERR_INTERNAL;
}

CEventBase::~CEventBase()
{
	for(map<int, CHandler*>::iterator iter = this->_sockets.begin(); iter != this->_sockets.end(); ++iter)
		delete iter->second;
	this->_sockets.clear();

	if(s_epollFd >= 0){
		close(s_epollFd);
		s_epollFd = -1;
	}
}

void CEventBase::_timer()
{
	map<int, CHandler*>::iterator iter = this->_sockets.begin();
	while(iter != this->_sockets.end()){
		CHandler* handler = iter->second;
		if((handler->_timeout > 0) && (handler->_time + handler->_timeout < g_timeNow)){
			std::map<int, CHandler*>::iterator iterToErase = iter;
			++iter;

			CHandler* handler = iterToErase->second;
			this->_close(iterToErase->first, handler, VAS_REASON_TIMEOUT);
		}
		else{
			++iter;
		}
	}
}

void CEventBase::_add(int fd, CHandler* handler)
{
	map<int, CHandler*>::iterator iter = this->_sockets.find(fd);
	if(this->_sockets.end() != iter){
		log_error("cannot add fd %d when its already there", fd);
		return;
	}

	this->_sockets[fd] = handler;

	struct epoll_event ev = {0};
	ev.events = EPOLLET;

	if(NULL != dynamic_cast<CHandler_Listener*>(handler)){
		ev.events = EPOLLIN|EPOLLET;
	}
	else{
		ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
	}
	
	ev.data.fd = fd;
	if(epoll_ctl(s_epollFd, EPOLL_CTL_ADD, fd, &ev) < 0)
		log_error("failed to add fd %d to epoll", fd);
}

void CEventBase::_close(int fd, CHandler*& handler, VAS_REASON reason)
{
	if(this->_sockets.find(fd) == this->_sockets.end()){
		log_error("no handler found for socket %d [_close]", fd);
		return;
	}

	::close(fd);
	handler->_onClosed(reason);
	delete handler;
	handler = NULL;
	this->_sockets.erase(fd);
}

void CEventBase::_addBuffer(int fd, CBuffer* buffer)
{
	map<int, CHandler*>::iterator iter = this->_sockets.find(fd);
	if(this->_sockets.end() == iter){
		log_error("no handler found for socket %d [_addBuffer]", fd);
		delete buffer;
		return;
	}

	CHandler* handler = iter->second;
	if(NULL != handler->_output){
		handler->_output->append(buffer);
		if(!CHelper::Socket::write(fd, handler->_output))
			this->_close(fd, handler, VAS_REASON_SERVER_CLOSED);
	}
	if(NULL != handler)
		handler->onWritten();
	
	delete buffer;
}

