/*
 * VasClientHandler.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_HANDLER_CLIENT_HANDLER_H_
#define INCLUDE_HANDLER_CLIENT_HANDLER_H_

#include "handler/VasAbstractHandler.h"
#include "core/VasBuffer.h"

class VasClientHandler : public VasAbstractHandler {
public:
	VasClientHandler();
	virtual ~VasClientHandler();

	virtual void onConnected(int fd) = 0;
	virtual void onDataArrived(int fd, VasBuffer *buffer) = 0;
};

#endif /* INCLUDE_HANDLER_CLIENT_HANDLER_H_ */
