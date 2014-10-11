/*
 * event_base.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: chenrui
 */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>
#include "event_base.h"
#include "helper.h"

using namespace vas;
using namespace std;

EventBase::EventBase() : _state(STATE_STOPPED) {
	this->_epollFd = ::epoll_create(64);
	if(this->_epollFd < 0){
		VAS_LOGGER_ERROR("cannot create epoll file descriptor");
		exit(-1);
	}

    if(!Helper::Application::setSignalHandler(SIGPIPE, SIG_IGN)){
    	VAS_LOGGER_ERROR("failed to set SIGPIPE to SIG_IGN");
        exit(-1);
    }
    if(!Helper::Application::setSignalHandler(SIGINT, EventBase::stop)){
    	VAS_LOGGER_ERROR("failed to set SIGINT to EventBase::stop");
        exit(-1);
    }
}

EventBase::~EventBase(){
	if(this->_epollFd >= 0){
		::close(this->_epollFd);
		this->_epollFd = -1;
	}
}

EventBase* EventBase::instance(){
	static EventBase self;
	return &self;
}

void EventBase::stop(int sig){
	EventBase::instance()->_state = STATE_STOPPING;
}

void EventBase::dispatch(){
	this->_state = STATE_RUNNING;
	this->notifyObserver(EVENT_STARTED, NULL);

	while(STATE_RUNNING == this->_state){
		if(Helper::Clock::sync() > 0)
			this->notifyObserver(EVENT_TIMER, NULL);

		struct epoll_event events[64];
		int nfds = epoll_wait(this->_epollFd, events, 64, 1000);
		if (nfds < 0)
			break;

		for(int i = 0; i < nfds; ++i){
			if(events[i].events & EPOLLIN){
				if(this->_listeners.find(events[i].data.fd) != this->_listeners.end())
					this->_doAccept(events[i].data.fd);
				else
					this->_doRead(events[i].data.fd);
			}
			else if(events[i].events & EPOLLOUT){
				this->_doWrite(events[i].data.fd);
			}
		}
	}
	this->_doShutdown();
	this->_state = STATE_STOPPED;
}

//////////////////////////////////////////////////////////////////////

void EventBase::addObserver(Event event, RoleObserver* observer){
	switch(event){
		case EVENT_STARTED:
			this->_observersForEventStarted.push_back(observer);
			break;
		case EVENT_STOPPED:
			this->_observersForEventStopped.push_back(observer);
			break;
		case EVENT_TIMER:
			this->_observersForEventTimer.push_back(observer);
			break;
		case EVENT_LISTENER_REGISTERED:
			this->_observersForEventListenerRegistered.push_back(observer);
			break;
		case EVENT_LISTENER_UNREGISTERED:
			this->_observersForEventListenerUnregistered.push_back(observer);
			break;
		case EVENT_SERVICE_REGISTERED:
			this->_observersForEventServiceRegistered.push_back(observer);
			break;
		case EVENT_SERVICE_UNREGISTERED:
			this->_observersForEventServiceUnregistered.push_back(observer);
			break;
		case EVENT_SERVICE_DATA_ARRIVED:
			this->_observersForEventServiceDataArrived.push_back(observer);
			break;
		case EVENT_SERVICE_DATA_SENT:
			this->_observersForEventServiceDataSent.push_back(observer);
			break;
		default:
			VAS_LOGGER_ERROR("trying to add observer for an unknown event=%d", event);
			break;
	}
}

void EventBase::removeObserver(Event event, RoleObserver* observer){
	switch(event){
		case EVENT_STARTED:
			this->_observersForEventStarted.remove(observer);
			break;
		case EVENT_STOPPED:
			this->_observersForEventStopped.remove(observer);
			break;
		case EVENT_TIMER:
			this->_observersForEventTimer.remove(observer);
			break;
		case EVENT_LISTENER_REGISTERED:
			this->_observersForEventListenerRegistered.remove(observer);
			break;
		case EVENT_LISTENER_UNREGISTERED:
			this->_observersForEventListenerUnregistered.remove(observer);
			break;
		case EVENT_SERVICE_DATA_ARRIVED:
			this->_observersForEventServiceDataArrived.remove(observer);
			break;
		case EVENT_SERVICE_DATA_SENT:
			this->_observersForEventServiceDataSent.remove(observer);
			break;
		default:
			VAS_LOGGER_ERROR("trying to remove observer for an unknown event=%d", event);
			break;
	}
}

void EventBase::notifyObserver(Event event, void* ctx){
	switch(event){
		case EVENT_STARTED:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventStarted.begin(); iter != this->_observersForEventStarted.end(); ++iter)
				(*iter)->onStarted();
			break;
		case EVENT_STOPPED:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventStopped.begin(); iter != this->_observersForEventStopped.end(); ++iter)
				(*iter)->onStopped();
			break;
		case EVENT_TIMER:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventTimer.begin(); iter != this->_observersForEventTimer.end(); ++iter)
				(*iter)->onTimer();
			break;
		case EVENT_LISTENER_REGISTERED:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventListenerRegistered.begin(); iter != this->_observersForEventListenerRegistered.end(); ++iter)
				(*iter)->onListenerRegistered((RoleListener*)ctx);
			break;
		case EVENT_LISTENER_UNREGISTERED:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventListenerUnregistered.begin(); iter != this->_observersForEventListenerUnregistered.end(); ++iter)
				(*iter)->onListenerUnregistered((RoleListener*)ctx);
			break;
		case EVENT_SERVICE_REGISTERED:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventServiceRegistered.begin(); iter != this->_observersForEventServiceRegistered.end(); ++iter)
				(*iter)->onServiceRegistered((RoleService*)ctx);
			break;
		case EVENT_SERVICE_UNREGISTERED:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventServiceUnregistered.begin(); iter != this->_observersForEventServiceUnregistered.end(); ++iter)
				(*iter)->onServiceUnregistered((RoleService*)ctx);
			break;
		case EVENT_SERVICE_DATA_ARRIVED:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventServiceDataArrived.begin(); iter != this->_observersForEventServiceDataArrived.end(); ++iter)
				(*iter)->onServiceDataArrived((RoleService*)ctx);
			break;
		case EVENT_SERVICE_DATA_SENT:
			for(list<RoleObserver*>::iterator iter = this->_observersForEventServiceDataSent.begin(); iter != this->_observersForEventServiceDataSent.end(); ++iter)
				(*iter)->onServiceDataSent((RoleService*)ctx);
			break;
		default:
			VAS_LOGGER_ERROR("trying to notify an unknown event=%d", event);
			break;
	}
}

void EventBase::registerListener(RoleListener* listener){
	struct epoll_event ev = {0};
	ev.events = EPOLLIN|EPOLLET;
	ev.data.fd = listener->getFd();
	if(::epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, listener->getFd(), &ev) < 0){
	    VAS_LOGGER_ERROR("failed to add listener %d to epoll", listener->getFd());
	    return;
	}
	this->_listeners[listener->getFd()] = listener;
	this->notifyObserver(EVENT_LISTENER_REGISTERED, listener);
}

void EventBase::unregisterListener(RoleListener* listener){
	::close(listener->getFd());
	this->_listeners.erase(listener->getFd());
	listener->onClosed();
	this->notifyObserver(EVENT_LISTENER_UNREGISTERED, listener);
}

void EventBase::registerService(RoleService* service){
	struct epoll_event ev = {0};
	ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
	ev.data.fd = service->getFd();
	if(::epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, service->getFd(), &ev) < 0){
		VAS_LOGGER_ERROR("failed to add service %d to epoll", service->getFd());
		return;
	}
	this->_services[service->getFd()] = service;
	this->notifyObserver(EVENT_SERVICE_REGISTERED, service);
}

void EventBase::unregisterService(RoleService* service){
	::close(service->getFd());
	service->disconnect();
	this->_services.erase(service->getFd());
	this->notifyObserver(EVENT_SERVICE_UNREGISTERED, service);
}

RoleService* EventBase::findService(int fd){
	map<int, RoleService*>::iterator iter = this->_services.find(fd);
	if(iter == this->_services.end())
		return NULL;
	return iter->second;
}

//////////////////////////////////////////////////////////////////////
void EventBase::_doAccept(int fd){
	map<int, RoleListener*>::iterator iter = this->_listeners.find(fd);
	if(iter == this->_listeners.end()){
		VAS_LOGGER_ERROR("unable to find listener on fd = %d", fd);
		return;
	}

	RoleListener* listener = iter->second;
	int clientFd = -1;
	while((clientFd = Helper::Socket::accept(fd)) >= 0)
		listener->onAccepted(clientFd);
}

void EventBase::_doRead(int fd){
	map<int, RoleService*>::iterator iter = this->_services.find(fd);
	if(iter == this->_services.end()){
		VAS_LOGGER_ERROR("unable to find service on fd = %d", fd);
		return;
	}

	RoleService* service = iter->second;
	service->read();
}

void EventBase::_doWrite(int fd){
	map<int, RoleService*>::iterator iter = this->_services.find(fd);
	if(iter == this->_services.end()){
		VAS_LOGGER_ERROR("unable to find service on fd = %d", fd);
		return;
	}

	RoleService* service = iter->second;
	if(service->getState() == RoleService::STATE_CONNECTING)
		service->connect();
	else
		service->write();
}

void EventBase::_doShutdown(){
	for(map<int, RoleService*>::iterator iter = this->_services.begin(); iter != this->_services.end();)
		this->unregisterService((iter++)->second);

	for(map<int, RoleListener*>::iterator iter = this->_listeners.begin(); iter != this->_listeners.end();)
		this->unregisterListener((iter++)->second);

	this->notifyObserver(EVENT_STOPPED, NULL);
}
