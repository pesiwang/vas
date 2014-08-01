#ifndef _VAS_HANDLER_CLIENT_H
#define _VAS_HANDLER_CLIENT_H

#include "handler.h"

class CHandler_Client : public CHandler
{
public:
	CHandler_Client(int fd, int timeout);
	virtual ~CHandler_Client();

	virtual void onRead();
	virtual void onWritten();

protected:
	virtual void _onConnected() = 0;
	virtual void _onData() = 0;
	virtual void _onClosed(VAS_REASON reason);

protected:
	bool _isConnected;
};

#endif
