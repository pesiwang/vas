/*
 * echo_observer.cpp
 *
 *  Created on: Oct 10, 2014
 *      Author: chenrui
 */

#include "echo_observer.h"
#include "vas/helper.h"
#include "vas/event_base.h"

using namespace vas;
using namespace std;

EchoObserver::EchoObserver(){
}

EchoObserver::~EchoObserver(){
}

EchoObserver* EchoObserver::instance(){
	static EchoObserver self;
	return &self;
}

void EchoObserver::onStarted(){
	VAS_LOGGER_DEBUG("onStarted");
}

void EchoObserver::onStopped(){
	VAS_LOGGER_DEBUG("onStopped");
}

void EchoObserver::onTimer(){
	VAS_LOGGER_DEBUG("onTimer, clock = %d", Helper::Clock::now());
	for(map<Service*, time_t>::iterator iter = this->_services.begin(); iter != this->_services.end();){
		if((iter->second + 5) < Helper::Clock::now()){
			VAS_LOGGER_DEBUG("closing timeout service = %d", iter->first->getFd());
			EventBase::instance()->unregisterService((iter++)->first);
		}
		else{
			iter++;
		}
	}
}

void EchoObserver::onListenerRegistered(Listener* listener){
	VAS_LOGGER_DEBUG("onListenerRegistered, listener = %d", listener->getFd());
}

void EchoObserver::onListenerUnregistered(Listener* listener){
	VAS_LOGGER_DEBUG("onListenerUnregistered, listener = %d", listener->getFd());
	delete listener;
}

void EchoObserver::onServiceRegistered(Service* service){
	VAS_LOGGER_DEBUG("onServiceRegistered, service = %d, total = %d", service->getFd(), this->_services.size());
	this->_services[service] = Helper::Clock::now();
}

void EchoObserver::onServiceUnregistered(Service* service){
	VAS_LOGGER_DEBUG("onServiceUnregistered, service = %d, total = %d", service->getFd(), this->_services.size());
	this->_services.erase(service);
	delete service;
}

void EchoObserver::onServiceDataArrived(Service* service){
	VAS_LOGGER_DEBUG("onServiceDataArrived, service = %d, input size = %d", service->getFd(), service->getInput()->size());
	map<Service*, time_t>::iterator iter = this->_services.find(service);
	if(iter == this->_services.end()){
		VAS_LOGGER_ERROR("no service found in observer!!!");
		return;
	}

	iter->second = Helper::Clock::now();
}

void EchoObserver::onServiceDataSent(Service* service){
	VAS_LOGGER_DEBUG("onServiceDataSent, service = %d, output size = %d", service->getFd(), service->getOutput()->size());
	map<Service*, time_t>::iterator iter = this->_services.find(service);
	if(iter == this->_services.end()){
		VAS_LOGGER_ERROR("no service found in observer!!!");
		return;
	}

	iter->second = Helper::Clock::now();
}
