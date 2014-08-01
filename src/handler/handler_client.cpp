#include "handler/handler_client.h"

CHandler_Client::CHandler_Client(int fd, int timeout) : CHandler(fd, timeout)
{
}

CHandler_Client::~CHandler_Client()
{
}

void CHandler_Client::onRead()
{
	CHandler::onRead();

	if(this->_input->size() > 0)
		this->_onData();
}

void CHandler_Client::onWritten()
{
	CHandler::onWritten();

	if(!this->_isConnected){
		this->_isConnected = true;
		this->_onConnected();
	}
}

//////////////////////////////////////////////

void CHandler_Client::_onClosed(VAS_REASON reason)
{
	this->_fd = -1;
	this->_isConnected = false;
}
