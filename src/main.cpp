#include <iostream>
#include <string.h>
#include <signal.h>
#include "definition.h"
#include "config.h"
#include "event_base.h"
#include "handler/logic/handler_listener_echo.h"

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

		int listenerFd = CEventBase::Helper::createServerSocket(CConfig::instance()->network.host, CConfig::instance()->network.port);
		if(listenerFd < 0)
			throw VAS_ERR_INTERNAL;

		CEventBase::instance()->add(listenerFd, new CHandler_Listener_Echo(listenerFd), VAS_HANDLER_ROLE_LISTENER);
		
		int clientFd = CEventBase::Helper::createClientSocket("localhost", 80);
		if(clientFd < 0)
			throw VAS_ERR_INTERNAL;

		CEventBase::instance()->add(clientFd, new CHandler_Listener_Echo

		CEventBase::instance()->start();
		CEventBase::release();
		CConfig::release();
	}
	catch(VAS_ERROR &error){
		cout<<"catch a error"<<endl;
	}

	return 0;
}
