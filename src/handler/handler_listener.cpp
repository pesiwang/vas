#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "handler/handler_listener.h"
#include "event_base.h"

CHandler_Listener::CHandler_Listener(int fd) : CHandler(fd, 0)
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

CHandler_Listener::~CHandler_Listener()
{
}

bool CHandler_Listener::onRead()
{
	if(!CHandler::onRead())
		return false;

	int clientFd, sin_len = sizeof(struct sockaddr);
    struct sockaddr_in sin;
    while((clientFd = accept(this->_fd, (struct sockaddr *)&sin, (socklen_t *)&sin_len)) >= 0){
		CEventBase::Helper::setNonBlocking(clientFd);
		this->onAccepted(clientFd);
	}
	return true;
}
