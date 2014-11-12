/*
 * observer.cpp
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#include "vas/observer.h"

using namespace vas;

Observer::Observer(){
}

Observer::~Observer(){
}

void Observer::onStarted(){
}

void Observer::onStopped(){
}

void Observer::onTimer(){
}

void Observer::onListenerRegistered(Listener* listener){
}

void Observer::onListenerUnregistered(Listener* listener){
}

void Observer::onServiceRegistered(Service* service){
}

void Observer::onServiceUnregistered(Service* service){
}

void Observer::onServiceDataArrived(Service* service){
}

void Observer::onServiceDataSent(Service* service){
}
