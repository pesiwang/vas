/*
 * role_service.h
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#ifndef VAS_ROLE_SERVICE_H_
#define VAS_ROLE_SERVICE_H_

#include "buffer.h"

namespace vas
{
	class RoleService
	{
	public:
		enum State
		{
			STATE_DISCONNECTED = 0,
			STATE_CONNECTING,
			STATE_CONNECTED
		};

	public:
		RoleService(int fd);
		virtual ~RoleService();

		int getFd() const;
		Buffer* getInput();
		Buffer* getOutput();
		State getState();

	public:
		void connect();
		void disconnect();
		void read();
		void write();

		virtual void onConnected() = 0;
		virtual void onDataArrived() = 0;
		virtual void onDisconnected() = 0;

	protected:
		int _fd;
		State _state;
		Buffer* _input, *_output;
	};
}


#endif /* VAS_ROLE_SERVICE_H_ */
