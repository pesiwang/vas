/*
 * MyClientHandler.cpp
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */


#include "MyClientHandler.h"
#include "core/VasEventbase.h"
#include "core/VasHelper.h"

MyClientHandler::MyClientHandler() {

}

MyClientHandler::~MyClientHandler() {

}

void MyClientHandler::onConnected(int fd) {
	VAS_LOGGER_DEBUG("client %d connected to remote", fd);

	VasBuffer buffer;
	buffer.write("GET /\r\n\r\n", 9);
	VasEventbase::instance()->writeSocket(fd, &buffer);
}

void MyClientHandler::onDataArrived(int fd, VasBuffer *buffer) {
	VAS_LOGGER_DEBUG("client got data len = %d", buffer->size());
	buffer->shrink(buffer->size());
}

void MyClientHandler::onClosed(int fd) {
	VAS_LOGGER_DEBUG("client %d closed", fd);
}


