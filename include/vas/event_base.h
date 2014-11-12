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
#include "listener.h"
#include "observer.h"
#include "service.h"

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

		void addObserver(Event event, Observer* observer);
		void removeObserver(Event event, Observer* observer);
		void notifyObserver(Event event, void* ctx);

		void registerListener(Listener* listener);
		void unregisterListener(Listener* listener);

		void registerService(Service* service);
		void unregisterService(Service* service);

		Service* findService(int fd);

	private:
		EventBase();
		virtual ~EventBase();

		void _doAccept(int fd);
		void _doRead(int fd);
		void _doWrite(int fd);
		void _doShutdown();

	private:
		std::list<Observer*> _observersForEventStarted;
		std::list<Observer*> _observersForEventStopped;
		std::list<Observer*> _observersForEventTimer;
		std::list<Observer*> _observersForEventListenerRegistered;
		std::list<Observer*> _observersForEventListenerUnregistered;
		std::list<Observer*> _observersForEventServiceRegistered;
		std::list<Observer*> _observersForEventServiceUnregistered;
		std::list<Observer*> _observersForEventServiceDataArrived;
		std::list<Observer*> _observersForEventServiceDataSent;

		std::map<int, Listener*> _listeners;
		std::map<int, Service*> _services;
		State _state;
		int _epollFd;
	};
}
#endif /* VAS_EVENTBASE_H_ */
