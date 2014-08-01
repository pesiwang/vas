#ifndef _VAS_HELPER_H
#define _VAS_HELPER_H

class CHelper
{
public:
	class Socket
	{
	public:
		static int listen(const char* addr, unsigned short port);
		static int connect(const char* addr, unsigned short port);
		static int accept(int listenerFd);
	};

	class Application
	{
	public:
		static void daemonize();
		static bool setFileLimit(int limit);
	};
};

#endif
