#ifndef _VAS_EVENT_BASE_H
#define _VAS_EVENT_BASE_H

#include <map>
#include <queue>
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
	struct JOB
	{
		VAS_COMMAND cmd;
		int fd;
		union{
			CBuffer* buffer;
			CHandler* handler;
		}payload;
	};
public:
	static CEventBase* instance();
	static void release();

	void start();
	void stop();
	void add(const JOB& job);
	void commit();

protected:
	CEventBase();
	virtual ~CEventBase();

	void _add(int fd, CHandler* handler);
	void _close(int fd, CHandler*& handler, VAS_REASON reason);
	void _addBuffer(int fd, CBuffer* buffer);
	void _timer();

protected:
	static CEventBase* _instance;
	volatile EventBaseStatus _status;

	std::map<int, CHandler*> _sockets;
	std::queue<JOB> _jobs;
};

#endif
