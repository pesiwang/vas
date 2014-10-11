/*
 * role_observer.cpp
 *
 *  Created on: Oct 9, 2014
 *      Author: chenrui
 */

#include "role_observer.h"

using namespace vas;

RoleObserver::RoleObserver(){
}

RoleObserver::~RoleObserver(){
}

void RoleObserver::onStarted(){
}

void RoleObserver::onStopped(){
}

void RoleObserver::onTimer(){
}

void RoleObserver::onListenerRegistered(RoleListener* listener){
}

void RoleObserver::onListenerUnregistered(RoleListener* listener){
}

void RoleObserver::onServiceRegistered(RoleService* service){
}

void RoleObserver::onServiceUnregistered(RoleService* service){
}

void RoleObserver::onServiceDataArrived(RoleService* service){
}

void RoleObserver::onServiceDataSent(RoleService* service){
}
