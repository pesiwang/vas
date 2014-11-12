/*
 * exception.h
 *
 *  Created on: Oct 8, 2014
 *      Author: chenrui
 */

#ifndef VAS_EXCEPTION_H_
#define VAS_EXCEPTION_H_

#include <exception>
#include <string>

namespace vas
{
	class Exception : public std::exception
	{
	public:
		Exception(std::string msg);
		virtual ~Exception() throw();
		const char* what() const throw();

	private:
		std::string _msg;
	};
}
#endif /* VAS_EXCEPTION_H_ */
