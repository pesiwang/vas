/*
 * MyListenerHandler.cpp
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#include "MyListenerHandler.h"
#include "core/VasHelper.h"
#include "core/VasEventbase.h"

MyListenerHandler::MyListenerHandler() {
	// TODO Auto-generated constructor stub

}

MyListenerHandler::~MyListenerHandler() {
	// TODO Auto-generated destructor stub
}

void MyListenerHandler::onAccepted(int fd) {
	VAS_LOGGER_DEBUG("client %d accepted", fd);

	VasEventbase::instance()->addServer(fd, &(this->_serverHandler), 3);
}

void MyListenerHandler::onClosed(int fd) {
	VAS_LOGGER_DEBUG("client %d closed", fd);
}
