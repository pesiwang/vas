/*
 * VasException.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_CORE_VAS_EXCEPTION_H_
#define INCLUDE_CORE_VAS_EXCEPTION_H_

#include <exception>
#include <string>

class VasException: public std::exception {
public:
	VasException(std::string msg);
	virtual ~VasException() throw ();
	const char* what() const throw ();

private:
	std::string _msg;
};

#endif /* INCLUDE_CORE_VAS_EXCEPTION_H_ */
