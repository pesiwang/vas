#include "handler/logic/handler_server_echo.h"
#include "event_base.h"

CHandler_Server_Echo::CHandler_Server_Echo(int fd, int timeout) : CHandler_Server(fd, timeout)
{
}

CHandler_Server_Echo::~CHandler_Server_Echo()
{
}

///////////////////////////////////////////

void CHandler_Server_Echo::_onData()
{
	CBuffer *buffer = new CBuffer(this->_input->size());
	buffer->append(this->_input);
	this->_input->shrink(this->_input->size());

	CEventBase::JOB job;
	job.cmd = VAS_COMMAND_ADD_BUFFER;
	job.fd = this->_fd;
	job.payload.buffer = buffer;
	CEventBase::instance()->add(job);
}

void CHandler_Server_Echo::_onClosed(VAS_REASON reason)
{
	log_debug("CHandler_Server_Echo onClosed, fd = %d", this->_fd);
}
