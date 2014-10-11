/*
 * event_base.h
 *
 *  Created on: Oct 8, 2014
 *      Author: chenrui
 */

#ifndef VAS_EVENT_BASE_H_
#define VAS_EVENT_BASE_H_

#include <map>
#include <list>
#include "role_listener.h"
#include "role_observer.h"
#include "role_service.h"

namespace vas
{
	class EventBase
	{
	public:
		enum Event{
			EVENT_STARTED,
			EVENT_STOPPED,
			EVENT_TIMER,
			EVENT_LISTENER_REGISTERED,
			EVENT_LISTENER_UNREGISTERED,
			EVENT_SERVICE_REGISTERED,
			EVENT_SERVICE_UNREGISTERED,
			EVENT_SERVICE_DATA_ARRIVED,
			EVENT_SERVICE_DATA_SENT
		};

		enum State{
			STATE_STOPPED,
			STATE_RUNNING,
			STATE_STOPPING
		};
	public:
		static EventBase* instance();
		static void stop(int sig);
		void dispatch();

		void addObserver(Event event, RoleObserver* observer);
		void removeObserver(Event event, RoleObserver* observer);
		void notifyObserver(Event event, void* ctx);

		void registerListener(RoleListener* listener);
		void unregisterListener(RoleListener* listener);

		void registerService(RoleService* service);
		void unregisterService(RoleService* service);

		RoleService* findService(int fd);

	private:
		EventBase();
		virtual ~EventBase();

		void _doAccept(int fd);
		void _doRead(int fd);
		void _doWrite(int fd);
		void _doShutdown();

	private:
		std::list<RoleObserver*> _observersForEventStarted;
		std::list<RoleObserver*> _observersForEventStopped;
		std::list<RoleObserver*> _observersForEventTimer;
		std::list<RoleObserver*> _observersForEventListenerRegistered;
		std::list<RoleObserver*> _observersForEventListenerUnregistered;
		std::list<RoleObserver*> _observersForEventServiceRegistered;
		std::list<RoleObserver*> _observersForEventServiceUnregistered;
		std::list<RoleObserver*> _observersForEventServiceDataArrived;
		std::list<RoleObserver*> _observersForEventServiceDataSent;

		std::map<int, RoleListener*> _listeners;
		std::map<int, RoleService*> _services;
		State _state;
		int _epollFd;
	};
}
#endif /* VAS_EVENTBASE_H_ */
