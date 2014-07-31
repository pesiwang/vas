#include "handler/logic/handler_listener_echo.h"
#include "handler/logic/handler_server_echo.h"
#include "event_base.h"

CHandler_Listener_Echo::CHandler_Listener_Echo(int fd) : CHandler_Listener(fd)
{
}

CHandler_Listener_Echo::~CHandler_Listener_Echo()
{
}

void CHandler_Listener_Echo::onAccepted(int clientFd)
{
	CEventBase::instance()->add(clientFd, new CHandler_Server_Echo(clientFd, 60), VAS_HANDLER_ROLE_NORMAL);
}
