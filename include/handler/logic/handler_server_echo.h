#ifndef _VAS_HANDLER_SERVER_ECHO_H
#define _VAS_HANDLER_SERVER_ECHO_H

#include "handler/handler_server.h"

class CHandler_Server_Echo : public CHandler_Server
{
public:
	CHandler_Server_Echo(int fd, int timeout);
	virtual ~CHandler_Server_Echo();

protected:
	void _onData();
	void _onClosed(VAS_REASON reason);
};

#endif
