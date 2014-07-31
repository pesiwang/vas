#include "handler/handler_server.h"
#include "event_base.h"

CHandler_Server::CHandler_Server(int fd, int timeout) : CHandler(fd, timeout)
{
}

CHandler_Server::~CHandler_Server()
{
}

bool CHandler_Server::onRead()
{
	if(!CHandler::onRead())
		return false;

	if(this->_input->size() > 0)
		return this->onData();

	return true;
}
