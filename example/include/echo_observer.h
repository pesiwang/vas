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
#include "vas/observer.h"
#include "vas/listener.h"
#include "vas/service.h"

class EchoObserver : public vas::Observer
{
private:
	EchoObserver();
	virtual ~EchoObserver();

public:
	static EchoObserver* instance();

	void onStarted();
	void onStopped();
	void onTimer();

	void onListenerRegistered(vas::Listener* listener);
	void onListenerUnregistered(vas::Listener* listener);

	void onServiceRegistered(vas::Service* service);
	void onServiceUnregistered(vas::Service* service);
	void onServiceDataArrived(vas::Service* service);
	void onServiceDataSent(vas::Service* service);

private:
	std::map<vas::Service*, time_t> _services;
};

#endif /* ECHO_OBSERVER_H_ */
