/*
 * VasEventbase.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>
#include <set>
#include "core/VasEventbase.h"
#include "core/VasException.h"
#include "core/VasHelper.h"

using namespace std;

VasEventbase::VasEventbase() : _serviceState(ServiceStateStopped), _epollFd(-1) {
	VasHelper::Clock::sync();

	this->_epollFd = ::epoll_create(64);
	if (this->_epollFd < 0) {
		throw VasException("cannot create epoll file descriptor");
	}

	if(!VasHelper::Application::setSignalHandler(SIGPIPE, SIG_IGN)) {
		throw VasException("failed to set SIGPIPE to SIG_IGN");
	}

	if (!VasHelper::Application::setSignalHandler(SIGINT, VasEventbase::stop)) {
		throw VasException("failed to set SIGINT to VasEventbase::stop");
	}
}

VasEventbase::~VasEventbase() {
	if (this->_epollFd >= 0) {
		::close(this->_epollFd);
		this->_epollFd = -1;
	}
}

VasEventbase *VasEventbase::instance(){
	static VasEventbase self;
	return &self;
}

void VasEventbase::stop(int sig){
	VasEventbase::instance()->_serviceState = ServiceStateStopping;
}

bool VasEventbase::addListener(int fd, VasListenerHandler *handler) {
	if (this->_activeSockets.find(fd) != this->_activeSockets.end()) {
		VAS_LOGGER_ERROR("cannot add a listener socket on fd = %d", fd);
		return false;
	}
	struct epoll_event ev = {0};
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if(::epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &ev) < 0){
		VAS_LOGGER_ERROR("failed to add listener socket %d to epoll", fd);
		return false;
	}

	Socket* socket = new Socket();
	socket->handler = handler;
	socket->state = SocketStateConnected;
	socket->maxIdleTime = 0;
	socket->lastActiveTime = VasHelper::Clock::now();
	socket->type = SocketTypeListener;
	this->_activeSockets[fd] = socket;
	return true;
}

void VasEventbase::addRoutine(VasRoutineHandler *handler) {
	this->_routineHandlers.push_back(handler);
}

bool VasEventbase::addClient(int fd, VasClientHandler *handler, time_t maxIdleTime) {
	if (this->_activeSockets.find(fd) != this->_activeSockets.end()) {
		VAS_LOGGER_ERROR("cannot add a client socket on fd = %d", fd);
		return false;
	}
	struct epoll_event ev = {0};
	ev.events = EPOLLIN|EPOLLOUT;
	ev.data.fd = fd;
	if(::epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &ev) < 0){
		VAS_LOGGER_ERROR("failed to add client socket %d to epoll", fd);
		return false;
	}

	Socket* socket = new Socket();
	socket->handler = handler;
	socket->state = SocketStateConnecting;
	socket->lastActiveTime = VasHelper::Clock::now();
	socket->maxIdleTime = maxIdleTime;
	socket->type = SocketTypeClient;
	this->_activeSockets[fd] = socket;
	return true;
}

bool VasEventbase::addServer(int fd, VasServerHandler *handler, time_t maxIdleTime) {
	if (this->_activeSockets.find(fd) != this->_activeSockets.end()) {
		VAS_LOGGER_ERROR("cannot add a server socket on fd = %d", fd);
		return false;
	}
	struct epoll_event ev = {0};
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if(::epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &ev) < 0){
		VAS_LOGGER_ERROR("failed to add server socket %d to epoll", fd);
		return false;
	}

	Socket* socket = new Socket();
	socket->handler = handler;
	socket->state = SocketStateConnected;
	socket->lastActiveTime = VasHelper::Clock::now();
	socket->maxIdleTime = maxIdleTime;
	socket->type = SocketTypeServer;
	this->_activeSockets[fd] = socket;
	return true;
}

bool VasEventbase::connectSocket(int fd) {
	map<int, Socket* >::iterator iter = this->_activeSockets.find(fd);
	if (iter == this->_activeSockets.end()) {
		VAS_LOGGER_ERROR("cannot connect socket with fd = %d", fd);
		return false;
	}

	struct epoll_event ev = {0};
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if(::epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, fd, &ev) < 0){
		VAS_LOGGER_ERROR("failed to mod server socket %d to epoll", fd);
		this->removeSocket(fd);
		return false;
	}

	Socket *socket = iter->second;
	socket->state = SocketStateConnected;
	(static_cast<VasClientHandler *>(socket->handler))->onConnected(fd);
	return true;
}

bool VasEventbase::writeSocket(int fd, VasBuffer *buffer) {
	if (!VasHelper::Socket::write(fd, buffer)) {
		this->removeSocket(fd);
		return false;
	}

	struct epoll_event ev = {0};
	ev.data.fd = fd;
	if (buffer->size() > 0) {
		ev.events = EPOLLIN|EPOLLOUT;
	}
	else {
		ev.events = EPOLLIN;
	}

	if(::epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, fd, &ev) < 0){
		VAS_LOGGER_ERROR("failed to mod server socket %d to epoll", fd);
		this->removeSocket(fd);
		return false;
	}
	return true;
}

bool VasEventbase::removeSocket(int fd) {
	map<int, Socket* >::iterator iter = this->_activeSockets.find(fd);
	if (iter == this->_activeSockets.end()) {
		VAS_LOGGER_ERROR("cannot remove socket with fd = %d", fd);
		return false;
	}

	if (this->_closingSockets.end() != this->_closingSockets.find(fd)) {
		VAS_LOGGER_ERROR("BUG! fd = %d already in closingSockets", fd);
	}

	iter->second->state = SocketStateClosing;
	this->_closingSockets[fd] = iter->second;
	this->_activeSockets.erase(iter);
	return true;
}

void VasEventbase::dispatch() {
	this->_serviceState = ServiceStateRunning;
	VasHelper::Clock::sync();
	while (ServiceStateRunning == this->_serviceState) {
		struct epoll_event events[64];
		int nfds = epoll_wait(this->_epollFd, events, 64, 1000);
		if (nfds < 0)
			continue;

		for (int i = 0; i < nfds; ++i) {
			int targetFd = events[i].data.fd;
			uint32_t targetEvents = events[i].events;

			map<int, Socket* >::iterator iter = this->_activeSockets.find(targetFd);
			if ((iter == this->_activeSockets.end()) || (iter->second->state == SocketStateClosed) || (iter->second->state == SocketStateClosing))
				continue;

			Socket *socket = iter->second;
			socket->lastActiveTime = VasHelper::Clock::now();
			switch(socket->type) {
				case SocketTypeListener:
					if ((targetEvents & EPOLLIN) && (socket->state == SocketStateConnected)) {
						int clientFd = -1;
						while((clientFd = VasHelper::Socket::accept(targetFd)) >= 0)
							(static_cast<VasListenerHandler *>(socket->handler))->onAccepted(clientFd);
					}
					break;
				case SocketTypeClient:
					if ((targetEvents & EPOLLIN) && (socket->state == SocketStateConnected)) {
						if (VasHelper::Socket::read(targetFd, &(socket->input))) {
							(static_cast<VasClientHandler *>(socket->handler))->onDataArrived(targetFd, &(socket->input));
						}
						else {
							this->removeSocket(targetFd);
						}
					}
					else if ((targetEvents & EPOLLOUT) && ((socket->state == SocketStateConnecting) || (socket->state == SocketStateConnected))) {
						if (socket->state == SocketStateConnecting) {
							this->connectSocket(targetFd);
						}
						else {
							this->writeSocket(targetFd, &(socket->output));
						}
					}
					break;
				case SocketTypeServer:
					if ((targetEvents & EPOLLIN) && (socket->state == SocketStateConnected)) {
						if (VasHelper::Socket::read(targetFd, &(socket->input))) {
							(static_cast<VasServerHandler *>(socket->handler))->onDataArrived(targetFd, &(socket->input));
						}
						else {
							this->removeSocket(targetFd);
						}
					}
					else if ((targetEvents & EPOLLOUT) && (socket->state == SocketStateConnected)) {
						this->writeSocket(targetFd, &(socket->output));
					}
					break;
				default:
					break;
			}
		}

		this->_notifyRoutines();
		this->_cleanupRunloop();
	}

	this->_cleanupService();
	this->_serviceState = ServiceStateStopped;
}

/////////////////////////////////////////////////////////////////////
void VasEventbase::_cleanupRunloop() {
	time_t timeElapsed = VasHelper::Clock::sync();
	time_t timeNow = VasHelper::Clock::now();
	if (timeElapsed >= 1) {
		set<int> closingFds;
		for (map<int, Socket*>::iterator iter = this->_activeSockets.begin(); iter != this->_activeSockets.end(); ++iter) {
			if ((iter->second->maxIdleTime > 0) && (iter->second->state != SocketStateClosed) && (iter->second->state != SocketStateClosing)) {
				if ((timeNow - iter->second->lastActiveTime) > iter->second->maxIdleTime) {
					closingFds.insert(iter->first);
				}
			}
		}

		for (set<int>::iterator iter = closingFds.begin(); iter != closingFds.end(); ++iter) {
			this->removeSocket(*iter);
		}
	}

	for (map<int, Socket*>::iterator iter = this->_closingSockets.begin(); iter != this->_closingSockets.end(); ++iter) {
		int targetFd = iter->first;
		::close(targetFd);
		iter->second->handler->onClosed(targetFd);
		delete iter->second;
	}
	this->_closingSockets.clear();
}

void VasEventbase::_notifyRoutines() {
	for (list<VasRoutineHandler *>::iterator iter = this->_routineHandlers.begin(); iter != this->_routineHandlers.end(); ++iter) {
		(*iter)->onRoutine();
	}
}

void VasEventbase::_cleanupService() {
	set<int> closingFds;
	for (map<int, Socket* >::iterator iter = this->_activeSockets.begin(); iter != this->_activeSockets.end(); ++iter) {
		if (iter->second->state != SocketStateClosing)
			closingFds.insert(iter->first);
	}

	for (set<int>::iterator iter = closingFds.begin(); iter != closingFds.end(); ++iter) {
		this->removeSocket(*iter);
	}
	this->_cleanupRunloop();
}
