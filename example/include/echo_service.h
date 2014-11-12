/*
 * echo_service.h
 *
 *  Created on: Oct 10, 2014
 *      Author: chenrui
 */

#ifndef ECHO_SERVICE_H_
#define ECHO_SERVICE_H_

#include "vas/service.h"

class EchoService : public vas::Service
{
public:
	EchoService(int fd);
	virtual ~EchoService();

	void onConnected();
	void onDataArrived();
	void onDisconnected();
};

#endif /* ECHO_SERVICE_H_ */
