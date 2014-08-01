#include "handler/handler_server.h"
#include "event_base.h"

CHandler_Server::CHandler_Server(int fd, int timeout) : CHandler(fd, timeout)
{
}

CHandler_Server::~CHandler_Server()
{
}

void CHandler_Server::onRead()
{
	CHandler::onRead();

	if(this->_input->size() > 0)
		this->_onData();
}
