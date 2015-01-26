/*
 * MyServerHandler.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef EXAMPLE_INCLUDE_MYSERVERHANDLER_H_
#define EXAMPLE_INCLUDE_MYSERVERHANDLER_H_


#include "handler/VasServerHandler.h"
#include "core/VasBuffer.h"

class MyServerHandler: public VasServerHandler {
public:
	MyServerHandler();
	virtual ~MyServerHandler();

	void onDataArrived(int fd, VasBuffer *buffer);
	void onClosed(int fd);
};


#endif /* EXAMPLE_INCLUDE_MYSERVERHANDLER_H_ */
