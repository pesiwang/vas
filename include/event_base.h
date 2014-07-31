#ifndef _VAS_EVENT_BASE_H
#define _VAS_EVENT_BASE_H

#include <map>
#include "definition.h"
#include "handler.h"

enum EventBaseStatus
{
	EBS_STOPPED = 0,
	EBS_RUNNING,
	EBS_STOPPING
};

class CEventBase
{
public:
	class Helper
	{
	public:
		static void setNonBlocking(int fd);
		static void setReuseAddress(int fd);
		static void forkAsDaemon();
		static void setResourceLimit(int limit);
	};

public:
	static CEventBase* instance();
	static void release();

	void start();
	void stop();
	void add(int fd, CHandler* handler, VAS_HANDLER_ROLE role);
	void addSwap(int fd, CBuffer* buffer);

protected:
	CEventBase();
	virtual ~CEventBase();

	CHandler* _doRead(int fd);
	CHandler* _doWrite(int fd);
	void _doClose(int fd, VAS_REASON reason);
	void _doTimer();
	void _doBroadcast();

protected:
	static CEventBase* _instance;
	volatile EventBaseStatus _status;
	std::map<int, CHandler*> _sockets;
	std::map<int, CBuffer*> _swaps;
};

#endif
