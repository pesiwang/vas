/*
 * service.h
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#ifndef VAS_SERVICE_H_
#define VAS_SERVICE_H_

#include "buffer.h"

namespace vas
{
	class Service
	{
	public:
		enum State
		{
			STATE_DISCONNECTED = 0,
			STATE_CONNECTING,
			STATE_CONNECTED
		};

	public:
		Service(int fd);
		virtual ~Service();

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


#endif /* VAS_SERVICE_H_ */
