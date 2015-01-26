/*
 * main.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#include "core/VasEventbase.h"
#include "core/VasHelper.h"
#include "MyListenerHandler.h"
#include "MyClientHandler.h"

int main() {
	int listenerFd = VasHelper::Socket::listen("0.0.0.0", 1238);
	MyListenerHandler listenerHandler;

	int clientFd = VasHelper::Socket::connect("10.10.2.52", 80);
	MyClientHandler clientHandler;

	VasEventbase::instance()->addListener(listenerFd, &listenerHandler);
	VasEventbase::instance()->addClient(clientFd, &clientHandler, 30);
	VasEventbase::instance()->dispatch();
	return 0;
}
