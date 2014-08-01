#ifndef _VAS_HANDLER_SERVER_H
#define _VAS_HANDLER_SERVER_H

#include "handler.h"

class CHandler_Server : public CHandler
{
public:
	CHandler_Server(int fd, int timeout);
	virtual ~CHandler_Server();

	virtual void onRead();

protected:
	virtual void _onData() = 0;
};

#endif
