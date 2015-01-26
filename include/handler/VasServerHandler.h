/*
 * VasServerHandler.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_HANDLER_SERVER_HANDLER_H_
#define INCLUDE_HANDLER_SERVER_HANDLER_H_

#include "handler/VasAbstractHandler.h"
#include "core/VasBuffer.h"

class VasServerHandler : public VasAbstractHandler {
public:
	VasServerHandler();
	virtual ~VasServerHandler();

	virtual void onDataArrived(int fd, VasBuffer *buffer) = 0;
};

#endif /* INCLUDE_HANDLER_SERVER_HANDLER_H_ */
