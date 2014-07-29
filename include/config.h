#ifndef _VAS_CONFIG_H
#define _VAS_CONFIG_H

class CConfig
{
	class CConfigServer
	{
	public:
		int maxClients;
		int maxIdleTime;
		bool daemonize;
	};

	class CConfigNetwork
	{
	public:
		char host[16];
		unsigned short port;
	};

public:
	CConfigNetwork network;
	CConfigServer server;

public:
    static CConfig* instance();
    static void release();
	static void setPathName(const char* pathname);

protected:
	CConfig();
	virtual ~CConfig();

protected:
	static CConfig* _instance;
	static const char* _pathname;
};

#endif
