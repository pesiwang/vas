#ifndef _VAS_HANDLER_LISTENER__ECHO_H
#define _VAS_HANDLER_LISTENER__ECHO_H

#include "handler/handler_listener.h"

class CHandler_Listener_Echo : public CHandler_Listener
{
public:
	CHandler_Listener_Echo(int fd);
	virtual ~CHandler_Listener_Echo();

protected:
	void _onAccepted(int clientFd);
	void _onClosed(VAS_REASON reason);
};

#endif
