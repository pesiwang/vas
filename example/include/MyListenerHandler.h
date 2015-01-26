/*
 * MyListenerHandler.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef SRC_TEST_MYLISTENERHANDLER_H_
#define SRC_TEST_MYLISTENERHANDLER_H_

#include "handler/VasListenerHandler.h"
#include "MyServerHandler.h"

class MyListenerHandler: public VasListenerHandler {
public:
	MyListenerHandler();
	virtual ~MyListenerHandler();

	void onAccepted(int fd);
	void onClosed(int fd);

private:
	MyServerHandler _serverHandler;
};

#endif /* SRC_TEST_MYLISTENERHANDLER_H_ */
