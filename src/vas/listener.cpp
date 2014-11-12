/*
 * listener.cpp
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#include "vas/listener.h"

using namespace vas;

Listener::Listener(int fd) : _fd(fd){
}

Listener::~Listener(){
}

int Listener::getFd() const{
	return this->_fd;
}
