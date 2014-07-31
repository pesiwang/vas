#ifndef _VAS_HANDLER_CLIENT_H
#define _VAS_HANDLER_CLIENT_H

#include "handler.h"

class CHandler_Client : public CHandler
{
public:
	CHandler_Client(int fd, int timeout);
	virtual ~CHandler_Client();

	virtual bool onRead();
	virtual bool onWritten();
	virtual void onClosed(VAS_REASON reason);

	virtual bool onConnected() = 0;
	virtual bool onData() = 0;

protected:
	bool _isConnected;
};

#endif
