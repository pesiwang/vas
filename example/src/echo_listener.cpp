#include "echo_listener.h"
#include "echo_service.h"
#include "vas/helper.h"
#include "vas/event_base.h"

using namespace vas;

EchoListener::EchoListener(int fd) : Listener(fd){
}

EchoListener::~EchoListener(){
}

void EchoListener::onAccepted(int newFd){
	VAS_LOGGER_DEBUG("new socket %d accepted", newFd);
	EchoService* service = new EchoService(newFd);
	service->connect();
	EventBase::instance()->registerService(service);
}

void EchoListener::onClosed(){
}
