#ifndef _VAS_HANDLER_LISTENER_H
#define _VAS_HANDLER_LISTENER_H

#include "handler.h"

class CHandler_Listener : public CHandler
{
public:
	CHandler_Listener(int fd);
	virtual ~CHandler_Listener();

	virtual void onRead();

protected:
	virtual void _onAccepted(int clientFd) = 0;
};

#endif
