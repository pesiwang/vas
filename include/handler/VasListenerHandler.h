/*
 * VasListenerHandler.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_HANDLER_LISTENER_HANDLER_H_
#define INCLUDE_HANDLER_LISTENER_HANDLER_H_

#include "handler/VasAbstractHandler.h"

class VasListenerHandler : public VasAbstractHandler {
public:
	VasListenerHandler();
	virtual ~VasListenerHandler();

	virtual void onAccepted(int fd) = 0;
};

#endif /* INCLUDE_HANDLER_LISTENER_HANDLER_H_ */
