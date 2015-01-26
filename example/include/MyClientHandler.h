/*
 * MyClientHandler.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef EXAMPLE_INCLUDE_MYCLIENTHANDLER_H_
#define EXAMPLE_INCLUDE_MYCLIENTHANDLER_H_

#include "handler/VasClientHandler.h"
#include "core/VasBuffer.h"

class MyClientHandler: public VasClientHandler {
public:
	MyClientHandler();
	virtual ~MyClientHandler();

	void onConnected(int fd);
	void onDataArrived(int fd, VasBuffer *buffer);
	void onClosed(int fd);
};




#endif /* EXAMPLE_INCLUDE_MYCLIENTHANDLER_H_ */
