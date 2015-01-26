/*
 * VasAbstractHandler.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_HANDLER_ABSTRACT_HANDLER_H_
#define INCLUDE_HANDLER_ABSTRACT_HANDLER_H_

class VasAbstractHandler {
public:
	VasAbstractHandler();
	virtual ~VasAbstractHandler();

	virtual void onClosed(int fd) = 0;
};

#endif /* INCLUDE_HANDLER_ABSTRACT_HANDLER_H_ */
