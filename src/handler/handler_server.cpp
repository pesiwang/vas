#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "handler/handler_server.h"
#include "handler/handler_client.h"
#include "event_base.h"
#include "config.h"

using namespace std;

CHandler_Server::CHandler_Server(int fd, int timeout) : CHandler(fd, timeout)
{
	if(NULL != this->_input){
		delete this->_input;
		this->_input = NULL;
	}
	if(NULL != this->_output){
		delete this->_output;
		this->_output = NULL;
	}
}

CHandler_Server::~CHandler_Server()
{
}

bool CHandler_Server::onRead(map<int, CBuffer*>& results)
{
	if(!CHandler::onRead(results))
		return false;

	int clientFd, sin_len = sizeof(struct sockaddr);
    struct sockaddr_in sin;
    while((clientFd = accept(this->_fd, (struct sockaddr *)&sin, (socklen_t *)&sin_len)) >= 0){
		CEventBase::instance()->add(clientFd, new CHandler_Client(clientFd, CConfig::instance()->server.maxIdleTime), VAS_HANDLER_ROLE_NORMAL);
    }
	return true;
}
