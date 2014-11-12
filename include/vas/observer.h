/*
 * observer.h
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#ifndef VAS_OBSERVER_H_
#define VAS_OBSERVER_H_

namespace vas
{
	class Listener;
	class Service;
	class Observer
	{
	public:
		Observer();
		virtual ~Observer();

	public:
		virtual void onStarted();
		virtual void onStopped();
		virtual void onTimer();

		virtual void onListenerRegistered(Listener* listener);
		virtual void onListenerUnregistered(Listener* listener);

		virtual void onServiceRegistered(Service* service);
		virtual void onServiceUnregistered(Service* service);

		virtual void onServiceDataArrived(Service* service);
		virtual void onServiceDataSent(Service* service);
	};
}

#endif /* VAS_OBSERVER_H_ */
