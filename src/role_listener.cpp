/*
 * role_listener.cpp
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#include "role_listener.h"

using namespace vas;

RoleListener::RoleListener(int fd) : _fd(fd){
}

RoleListener::~RoleListener(){
}

int RoleListener::getFd() const{
	return this->_fd;
}
