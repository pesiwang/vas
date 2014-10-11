/*
 * echo_observer.h
 *
 *  Created on: Oct 10, 2014
 *      Author: chenrui
 */

#ifndef ECHO_OBSERVER_H_
#define ECHO_OBSERVER_H_

#include <map>
#include <time.h>
#include "vas/role_observer.h"
#include "vas/role_listener.h"
#include "vas/role_service.h"

class EchoObserver : public vas::RoleObserver
{
private:
	EchoObserver();
	virtual ~EchoObserver();

public:
	static EchoObserver* instance();

	void onStarted();
	void onStopped();
	void onTimer();

	void onListenerRegistered(vas::RoleListener* listener);
	void onListenerUnregistered(vas::RoleListener* listener);

	void onServiceRegistered(vas::RoleService* service);
	void onServiceUnregistered(vas::RoleService* service);
	void onServiceDataArrived(vas::RoleService* service);
	void onServiceDataSent(vas::RoleService* service);

private:
	std::map<vas::RoleService*, time_t> _services;
};

#endif /* ECHO_OBSERVER_H_ */
