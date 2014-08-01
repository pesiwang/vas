#ifndef _VAS_HANDLER_H
#define _VAS_HANDLER_H

#include <time.h>
#include "buffer.h"
#include "definition.h"

class CEventBase;
class CHandler
{
public:
	friend class CEventBase;

	CHandler(int fd, int timeout);
	virtual ~CHandler();

	virtual void onRead();
	virtual void onWritten();

protected:
	virtual void _onClosed(VAS_REASON reason) = 0;

protected:
	int _fd;
	time_t _time;
	int _timeout;
	CBuffer *_input, *_output;
};

#endif
