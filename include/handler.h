#ifndef _VAS_HANDLER_H
#define _VAS_HANDLER_H

#include <map>
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

	virtual bool onRead(std::map<int, CBuffer*>& results);
	virtual bool onWritten();
	virtual void onClosed(VAS_REASON reason);

protected:
	int _fd;
	time_t _time;
	int _timeout;
	CBuffer *_input, *_output;
};

#endif
