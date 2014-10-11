/*
 * role_listener.h
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#ifndef VAS_ROLE_LISTENER_H_
#define VAS_ROLE_LISTENER_H_

namespace vas
{
	class RoleListener
	{
	public:
		RoleListener(int fd);
		virtual ~RoleListener();

		int getFd() const;

	public:
		virtual void onAccepted(int newFd) = 0;
		virtual void onClosed() = 0;

	protected:
		int _fd;
	};
}

#endif /* VAS_ROLE_LISTENER_H_ */
