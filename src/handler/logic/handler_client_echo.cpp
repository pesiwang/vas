#include "handler/logic/handler_client_echo.h"
#include "event_base.h"

CHandler_Server_Echo::CHandler_Server_Echo(int fd, int timeout) : CHandler_Server(fd, timeout)
{
}

CHandler_Server_Echo::~CHandler_Server_Echo()
{
}

bool CHandler_Server_Echo::onData()
{
	CBuffer *buffer = new CBuffer(this->_input->size());
	buffer->append(this->_input);
	this->_input->shrink(this->_input->size());

	CEventBase::instance()->addSwap(this->_fd, buffer);
	return true;
}
