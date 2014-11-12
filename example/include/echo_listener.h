/*
 * echo_listener.h
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#ifndef ECHO_LISTENER_H_
#define ECHO_LISTENER_H_

#include "vas/listener.h"

class EchoListener : public vas::Listener
{
public:
	EchoListener(int fd);
	virtual ~EchoListener();

public:
	void onAccepted(int newFd);
	void onClosed();
};

#endif /* ECHO_LISTENER_H_ */
