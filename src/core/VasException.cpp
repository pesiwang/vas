/*
 * VasException.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */


#include "core/VasException.h"
using namespace std;

VasException::VasException(string msg) : _msg(msg) {
}

VasException::~VasException() throw () {
}

const char* VasException::what() const throw () {
	return this->_msg.c_str();
}
