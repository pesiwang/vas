/*
 * role_observer.h
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#ifndef VAS_ROLE_OBSERVER_H_
#define VAS_ROLE_OBSERVER_H_

namespace vas
{
	class RoleListener;
	class RoleService;
	class RoleObserver
	{
	public:
		RoleObserver();
		virtual ~RoleObserver();

	public:
		virtual void onStarted();
		virtual void onStopped();
		virtual void onTimer();

		virtual void onListenerRegistered(RoleListener* listener);
		virtual void onListenerUnregistered(RoleListener* listener);

		virtual void onServiceRegistered(RoleService* service);
		virtual void onServiceUnregistered(RoleService* service);

		virtual void onServiceDataArrived(RoleService* service);
		virtual void onServiceDataSent(RoleService* service);
	};
}

#endif /* VAS_ROLE_OBSERVER_H_ */
