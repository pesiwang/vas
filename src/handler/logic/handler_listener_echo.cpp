#include "handler/logic/handler_listener_echo.h"
#include "handler/logic/handler_server_echo.h"
#include "event_base.h"

CHandler_Listener_Echo::CHandler_Listener_Echo(int fd) : CHandler_Listener(fd)
{
}

CHandler_Listener_Echo::~CHandler_Listener_Echo()
{
}

////////////////////////////////////////////////////////

void CHandler_Listener_Echo::_onAccepted(int clientFd)
{
	CEventBase::JOB job;
	job.cmd = VAS_COMMAND_ADD_FD;
	job.fd = clientFd;
	job.payload.handler = new CHandler_Server_Echo(clientFd, 60);
	CEventBase::instance()->add(job);
}

void CHandler_Listener_Echo::_onClosed(VAS_REASON reason)
{
	log_debug("CHandler_Listener_Echo closed");
}
