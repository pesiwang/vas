#include <iostream>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "definition.h"
#include "config.h"
#include "event_base.h"
#include "handler/handler_server.h"

using namespace std;

static void handle_signal_int(int sig)
{
	CEventBase::instance()->stop();
}

int main(int argc, char* argv[])
{
	try{
		if(argc >= 2)
			CConfig::setPathName(argv[1]);

    	struct sigaction sa;
	    sa.sa_handler = SIG_IGN;
    	sa.sa_flags = 0;
	    if(sigemptyset(&sa.sa_mask) == -1 || sigaction(SIGPIPE, &sa, NULL) == -1)
    	    throw VAS_ERR_INTERNAL;

		sa.sa_handler = handle_signal_int;
	    if(sigemptyset(&sa.sa_mask) == -1 || sigaction(SIGINT, &sa, NULL) == -1)
    	    throw VAS_ERR_INTERNAL;

		CEventBase::Helper::setResourceLimit(CConfig::instance()->server.maxClients);
		if(CConfig::instance()->server.daemonize)
			CEventBase::Helper::forkAsDaemon();

		struct sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
		sin.sin_addr.s_addr = inet_addr(CConfig::instance()->network.host);
		sin.sin_family = AF_INET;
		sin.sin_port = htons(CConfig::instance()->network.port);
		int listenerFd = socket(AF_INET, SOCK_STREAM, 0);
		if(listenerFd < 0)
			throw VAS_ERR_INTERNAL;

		CEventBase::Helper::setReuseAddress(listenerFd);
		if((bind(listenerFd, (struct sockaddr *)&sin, sizeof(sin)) < 0) || (listen(listenerFd, SOMAXCONN) < 0))
			throw VAS_ERR_INTERNAL;
	
		CEventBase::instance()->add(listenerFd, new CHandler_Server(listenerFd, 0), VAS_HANDLER_ROLE_LISTENER);
		CEventBase::instance()->start();
		CEventBase::release();
		CConfig::release();
	}
	catch(VAS_ERROR &error){
		cout<<"catch a error"<<endl;
	}

	return 0;
}
