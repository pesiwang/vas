#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "definition.h"
#include "event_base.h"

using namespace std;

static int s_epollFd;
time_t g_timeNow = time(NULL);
time_t g_timeLast = g_timeNow;

CEventBase* CEventBase::_instance = NULL;

void CEventBase::Helper::setNonBlocking(int fd)
{
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void CEventBase::Helper::setReuseAddress(int fd)
{
	int optval = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval, sizeof(optval));
}

///////////////////////////////////////////////////////

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
			this->_doTimer();
			g_timeLast = g_timeNow;
		}

		int nfds = epoll_wait(s_epollFd, events, 64, 1000);
		if (nfds == -1)
			continue;

		for(int i = 0; i < nfds; ++i){
			if(events[i].events & EPOLLIN){
				CHandler *handler = this->_doRead(events[i].data.fd);
				if(handler){
					map<int, CBuffer*> results;
					if(!handler->onRead(results))
						this->_doClose(events[i].data.fd, VAS_REASON_SERVER_CLOSED);
					else
						this->_doBroadcast(results);
				}
			}
			if(events[i].events & EPOLLOUT){
				CHandler* handler = this->_doWrite(events[i].data.fd);
				if(handler){
					if(!handler->onWritten())
						this->_doClose(events[i].data.fd, VAS_REASON_SERVER_CLOSED);
				}
			}
        }
    }
	this->_status = EBS_STOPPED;
}

void CEventBase::stop()
{
	this->_status = EBS_STOPPING;
}

void CEventBase::add(int fd, CHandler* handler, VAS_HANDLER_ROLE role)
{
	map<int, CHandler*>::iterator iter = this->_sockets.find(fd);
	if(this->_sockets.end() != iter)
		throw VAS_ERR_INTERNAL;

	Helper::setNonBlocking(fd);
	this->_sockets[fd] = handler;

	struct epoll_event ev = {0};
	ev.events = EPOLLET;
	if(role == VAS_HANDLER_ROLE_LISTENER){
		ev.events = EPOLLIN|EPOLLET;
	}
	else{
		ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
	}
	
	ev.data.fd = fd;
	if(epoll_ctl(s_epollFd, EPOLL_CTL_ADD, fd, &ev) < 0)
		throw VAS_ERR_INTERNAL;
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

CHandler* CEventBase::_doRead(int fd)
{
	map<int, CHandler*>::iterator iter = this->_sockets.find(fd);
	if(iter == this->_sockets.end()){
		log_error("no handler found for socket %d [_doRead]", fd);
		return NULL;
	}

	CHandler* handler = iter->second;
	CBuffer* buffer = handler->_input;
	if(NULL == buffer)
		return handler;

	char buf[1024];
    int curr_read = 0;
    int total_read = 0;
    while((curr_read = read(fd, buf, 1024)) > 0){
		buffer->write(buf, curr_read);
        total_read += curr_read;
    }

	if((curr_read == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
		return handler;

    if((curr_read == -1) || (total_read == 0)){
		this->_doClose(fd, VAS_REASON_CLIENT_CLOSED);
        return NULL;
    }
	return handler;
}

CHandler* CEventBase::_doWrite(int fd)
{
	map<int, CHandler*>::iterator iter = this->_sockets.find(fd);
	if(iter == this->_sockets.end()){
		log_error("no handler found for socket %d [_doWrite]", fd);
		return NULL;
	}

	CHandler* handler = iter->second;
	CBuffer* buffer = handler->_output;
	if((NULL == buffer) || (buffer->size() == 0))
		return handler;

	bool blocked = false;
	char buf[1024];
	int curr_read = 0;
	while(!blocked && ((curr_read = buffer->peek(buf, 1024)) > 0)){
		int n_written = 0;
		int n_left = curr_read;
		while(n_left > 0){
			int written = write(fd, buf + n_written, n_left);
			if (written < n_left){
				if((written == -1) && (errno != EAGAIN) && (errno != EWOULDBLOCK)){
					this->_doClose(fd, VAS_REASON_CLIENT_CLOSED);
					return handler;
				}
               	
				if(written == -1){
					blocked = true;
					break;
				}
			}

			if(written > 0){
				n_written += written;
				n_left -= written;
			}
       	}

		if(n_written > 0)
			buffer->shrink(n_written);
	}

	return handler;
}

void CEventBase::_doTimer()
{
	map<int, CHandler*>::iterator iter = this->_sockets.begin();
	while(iter != this->_sockets.end()){
		CHandler* handler = iter->second;
		if((handler->_timeout > 0) && (handler->_time + handler->_timeout < g_timeNow)){
			std::map<int, CHandler*>::iterator iterToErase = iter;
			++iter;

			this->_doClose(iterToErase->first, VAS_REASON_TIMEOUT);
		}
		else{
			++iter;
		}
	}
}

void CEventBase::_doBroadcast(map<int, CBuffer*>& results)
{
	for(map<int, CBuffer*>::iterator iter = results.begin(); iter != results.end(); ++iter){
		int fd = iter->first;
		CBuffer* buffer = iter->second;

		map<int, CHandler*>::iterator iter = this->_sockets.find(fd);
		if(this->_sockets.end() == iter){
			log_error("socket %d not exists in _doBroadcast", fd);
			delete buffer;
			continue;
		}

		CHandler* handler = iter->second;
		handler->_output->append(buffer);

		handler = this->_doWrite(fd);
		if(handler)
			if(!handler->onWritten())
				this->_doClose(fd, VAS_REASON_SERVER_CLOSED);

		delete buffer;
	}
}

void CEventBase::_doClose(int fd, VAS_REASON reason)
{
	if(this->_sockets.find(fd) == this->_sockets.end()){
		log_error("no handler found for socket %d [_doClose]", fd);
		return;
	}

	CHandler* handler = this->_sockets[fd];

	close(fd);
	handler->onClosed(reason);
	delete handler;
	this->_sockets.erase(fd);
}
