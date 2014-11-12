#include "echo_service.h"
#include "vas/helper.h"

using namespace vas;

EchoService::EchoService(int fd) : Service(fd){
}

EchoService::~EchoService(){
}

void EchoService::onConnected(){
	VAS_LOGGER_DEBUG("service %d connected", this->_fd);
}

void EchoService::onDataArrived(){
	VAS_LOGGER_DEBUG("service %d got data", this->_fd);
	this->_output->append(this->_input);
	this->_input->shrink(this->_input->size());
	this->write();
}

void EchoService::onDisconnected(){
	VAS_LOGGER_DEBUG("service %d disconnected", this->_fd);
}
