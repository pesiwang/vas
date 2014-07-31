#ifndef _VAS_HANDLER_CLIENT_ECHO_H
#define _VAS_HANDLER_CLIENT_ECHO_H

#include "handler/handler_client.h"

class CHandler_Client_Echo : public CHandler_Client
{
public:
	CHandler_Client_Echo(int fd, int timeout);
	virtual ~CHandler_Client_Echo();

	bool onConnected();
	bool onData();
	void onClosed(VAS_REASON reason);
};

#endif
