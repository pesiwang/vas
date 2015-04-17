/*
 * VasRoutineHandler.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_HANDLER_ROUTINE_HANDLER_H_
#define INCLUDE_HANDLER_ROUTINE_HANDLER_H_

#include "handler/VasAbstractHandler.h"
#include "core/VasBuffer.h"

class VasRoutineHandler {
public:
	VasRoutineHandler();
	virtual ~VasRoutineHandler();

	virtual void onRoutine() = 0;
};

#endif /* INCLUDE_HANDLER_ROUTINE_HANDLER_H_ */
