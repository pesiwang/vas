/*
 * role_service.cpp
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#include <stdio.h>
#include "role_service.h"
#include "event_base.h"
#include "helper.h"

using namespace vas;

RoleService::RoleService(int fd) : _fd(fd), _state(STATE_DISCONNECTED), _input(0), _output(0){
	this->_input = new Buffer();
	this->_output = new Buffer();
}

RoleService::~RoleService(){
	if(NULL != this->_input){
		delete this->_input;
		this->_input = NULL;
	}
	if(NULL != this->_output){
		delete this->_output;
		this->_output = NULL;
	}
}

int RoleService::getFd() const{
	return this->_fd;
}

Buffer* RoleService::getInput(){
	return this->_input;
}

Buffer* RoleService::getOutput(){
	return this->_output;
}

RoleService::State RoleService::getState(){
	return this->_state;
}

///////////////////////////////////////////////////////////////////////

void RoleService::connect(){
	if(this->_state == STATE_CONNECTED){
		VAS_LOGGER_ERROR("invalid state(%d) on service(%d) detected when attempting to connect", this->_state, this->_fd);
		EventBase::instance()->unregisterService(this);
		return;
	}
	if((this->_state == STATE_CONNECTING) && !Helper::Socket::isOK(this->_fd)){
		VAS_LOGGER_ERROR("connecting failed on socket = %d", this->_fd);
		EventBase::instance()->unregisterService(this);
		return;
	}
	this->_state = STATE_CONNECTED;
	this->onConnected();
}

void RoleService::disconnect(){
	if(this->_state != STATE_DISCONNECTED){
		this->_state = STATE_DISCONNECTED;
		this->onDisconnected();
	}
}

void RoleService::read(){
	if(this->_state != STATE_CONNECTED){
		VAS_LOGGER_ERROR("invalid state(%d) on service(%d) detected when attempting to read", this->_state, this->_fd);
		EventBase::instance()->unregisterService(this);
		return;
	}

	if(!Helper::Socket::read(this->_fd, this->_input)){
		EventBase::instance()->unregisterService(this);
		return;
	}
	EventBase::instance()->notifyObserver(EventBase::EVENT_SERVICE_DATA_ARRIVED, this);
	this->onDataArrived();
}

void RoleService::write(){
	if(this->_state != STATE_CONNECTED){
		VAS_LOGGER_ERROR("invalid state(%d) on service(%d) detected when attempting to write", this->_state, this->_fd);
		EventBase::instance()->unregisterService(this);
		return;
	}

	if(this->_output->size() == 0)
		return;

	if(!Helper::Socket::write(this->_fd, this->_output)){
		VAS_LOGGER_ERROR("writing failed on service = %d", this->_fd);
		EventBase::instance()->unregisterService(this);
		return;
	}
	EventBase::instance()->notifyObserver(EventBase::EVENT_SERVICE_DATA_SENT, this);
}
