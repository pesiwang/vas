/*
 * listener.h
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#ifndef VAS_LISTENER_H_
#define VAS_LISTENER_H_

namespace vas
{
	class Listener
	{
	public:
		Listener(int fd);
		virtual ~Listener();

		int getFd() const;

	public:
		virtual void onAccepted(int newFd) = 0;
		virtual void onClosed() = 0;

	protected:
		int _fd;
	};
}

#endif /* VAS_LISTENER_H_ */
