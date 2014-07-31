#include "handler/handler_client.h"

CHandler_Client::CHandler_Client(int fd, int timeout) : CHandler(fd, timeout)
{
}

CHandler_Client::~CHandler_Client()
{
}

bool CHandler_Client::onRead()
{
	if(!CHandler::onRead())
		return false;

	if(this->_input->size() > 0)
		return this->onData();
	return true;
}

bool CHandler_Client::onWritten()
{
	if(!CHandler::onWritten())
		return false;

	if(!this->_isConnected){
		this->_isConnected = true;
		this->onConnected();
	}
	return true;
}

void CHandler_Client::onClosed(VAS_REASON reason)
{
	this->_fd = -1;
	this->_isConnected = false;
}
