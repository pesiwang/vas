#include <unistd.h>
#include "handler.h"

extern time_t g_timeNow;

CHandler::CHandler(int fd, int timeout) : _fd(fd), _timeout(timeout)
{
	this->_input = new CBuffer();
	this->_output = new CBuffer();
	this->_time = g_timeNow;
}

CHandler::~CHandler()
{
	if(NULL != this->_input){
		delete this->_input;
		this->_input = NULL;
	}
	if(NULL != this->_output){
		delete this->_output;
		this->_output = NULL;
	}
}

void CHandler::onRead()
{
	this->_time = g_timeNow;
}

void CHandler::onWritten()
{
	this->_time = g_timeNow;
}
