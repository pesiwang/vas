/*
 * exception.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: chenrui
 */

#include "vas/exception.h"

using namespace std;
using namespace vas;

Exception::Exception(string msg) : _msg(msg)
{
}

Exception::~Exception() throw ()
{
}

const char* Exception::what() const throw()
{
	return this->_msg.c_str();
}
