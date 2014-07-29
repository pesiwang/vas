#ifndef _VAS_HANDLER_CLIENT_H
#define _VAS_HANDLER_CLIENT_H

#include "handler.h"

class CHandler_Client : public CHandler
{
public:
	CHandler_Client(int fd, int timeout);
	virtual ~CHandler_Client();

	bool onRead(std::map<int, CBuffer*>& results);
};

#endif
