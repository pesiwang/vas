/*
 * VasEventbase.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_CORE_VAS_EVENTBASE_H_
#define INCLUDE_CORE_VAS_EVENTBASE_H_

#include <time.h>
#include "core/VasBuffer.h"
#include "handler/VasClientHandler.h"
#include "handler/VasListenerHandler.h"
#include "handler/VasServerHandler.h"
#include "handler/VasRoutineHandler.h"
#include <map>
#include <list>

class VasEventbase {
public:
	enum ServiceState {
		ServiceStateStopped,
		ServiceStateRunning,
		ServiceStateStopping
	};
	enum SocketState {
		SocketStateClosed = 0,
		SocketStateConnecting,
		SocketStateConnected,
		SocketStateClosing
	};
	enum SocketType {
		SocketTypeListener,
		SocketTypeServer,
		SocketTypeClient
	};

	class Socket {
	public:
		SocketType type;
		SocketState state;
		time_t lastActiveTime;
		time_t maxIdleTime;
		VasAbstractHandler *handler;
		VasBuffer input;
		VasBuffer output;
	};

public:
	static VasEventbase* instance();
	static void stop(int sig);

	bool addListener(int fd, VasListenerHandler *handler);
	bool addClient(int fd, VasClientHandler *handler, time_t maxIdleTime);
	bool addServer(int fd, VasServerHandler *handler, time_t maxIdleTime);
	void addRoutine(VasRoutineHandler *handler);
	bool connectSocket(int fd);
	bool writeSocket(int fd, VasBuffer *buffer);
	bool removeSocket(int fd);

	std::map<int, Socket *> getActiveSockets(SocketType type);

	void dispatch();

private:
	VasEventbase();
	virtual ~VasEventbase();


	void _cleanupRunloop();
	void _cleanupService();
	void _notifyRoutines();

private:
	ServiceState _serviceState;
	std::map<int, Socket*> _activeSockets;
	std::map<int, Socket*> _closingSockets;
	std::list<VasRoutineHandler *> _routineHandlers;
	int _epollFd;
};

#endif /* INCLUDE_CORE_VAS_EVENTBASE_H_ */
