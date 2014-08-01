#ifndef _VAS_HELPER_H
#define _VAS_HELPER_H

#include "buffer.h"

class CHelper
{
public:
	class Socket
	{
	public:
		static int listen(const char* addr, unsigned short port);
		static int connect(const char* addr, unsigned short port);
		static int accept(int listenerFd);
		static bool read(int fd, CBuffer* buffer);
		static bool write(int fd, CBuffer* buffer);
	};

	class Application
	{
	public:
		static void daemonize();
		static bool setFileLimit(int limit);
	};
};

#endif
