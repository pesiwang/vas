/*
 * MyServerHandler.cpp
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#include "MyServerHandler.h"
#include "core/VasEventbase.h"
#include "core/VasHelper.h"

MyServerHandler::MyServerHandler() {

}

MyServerHandler::~MyServerHandler() {

}

void MyServerHandler::onDataArrived(int fd, VasBuffer *buffer) {
	VasEventbase::instance()->writeSocket(fd, buffer);
	buffer->shrink(buffer->size());
}

void MyServerHandler::onClosed(int fd) {
	VAS_LOGGER_DEBUG("client %d closed", fd);
}
