/*
 * VasHelper.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include "core/VasHelper.h"
#include "core/VasException.h"
#include <jsoncpp/json/json.h>

using namespace std;

int VasHelper::Socket::create() {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		return -1;

	if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) < 0) {
		::close(fd);
		return -1;
	}
	return fd;
}

int VasHelper::Socket::accept(int listenerFd) {
	int sin_len = sizeof(struct sockaddr);
	struct sockaddr_in sin;
	int clientFd = ::accept(listenerFd, (struct sockaddr *) &sin,
			(socklen_t *) &sin_len);
	if (clientFd < 0)
		return -1;

	if (fcntl(clientFd, F_SETFL, fcntl(clientFd, F_GETFL) | O_NONBLOCK) < 0) {
		close(clientFd);
		return -1;
	}
	return clientFd;
}

int VasHelper::Socket::listen(const char* addr, unsigned short port) {
	int fd = VasHelper::Socket::create();
	if (fd < 0)
		return -1;
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	int optval = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval,
			sizeof(optval));
	if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) < 0) {
		close(fd);
		return -1;
	}
	if ((::bind(fd, (struct sockaddr *) &sin, sizeof(sin)) < 0)
			|| (::listen(fd, SOMAXCONN) < 0)) {
		close(fd);
		return -1;
	}
	return fd;
}

int VasHelper::Socket::connect(const char* addr, unsigned short port) {
	int fd = VasHelper::Socket::create();
	if (fd < 0)
		return -1;

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	int res = ::connect(fd, (struct sockaddr *) &sin, sizeof(sin));
	if ((res < 0) && (errno != EINPROGRESS)) {
		::close(fd);
		return -1;
	}
	return fd;
}

bool VasHelper::Socket::read(int fd, VasBuffer* buffer) {
	char buf[1024];
	int curr_read = 0;
	int total_read = 0;
	while ((curr_read = ::read(fd, buf, 1024)) > 0) {
		buffer->write(buf, curr_read);
		total_read += curr_read;
	}
	if ((curr_read == -1) && (errno == EAGAIN))
		return true;
	if ((curr_read == -1) || (total_read == 0)) {
		return false;
	}
	return true;
}

bool VasHelper::Socket::write(int fd, VasBuffer* buffer) {
	bool blocked = false;
	while (!blocked) {
		int curr_read = buffer->size() > 1024 ? 1024 : buffer->size();
		if (curr_read <= 0)
			break;
		char buf[1024];
		buffer->peek(buf, curr_read);
		int n_written = 0;
		int n_left = curr_read;
		while (n_left > 0) {
			int written = ::write(fd, buf + n_written, n_left);
			if (written < n_left) {
				if ((written == -1) && (errno != EAGAIN))
					return false;
				if (written == -1) {
					blocked = true;
					break;
				}
			}
			if (written > 0) {
				n_written += written;
				n_left -= written;
			}
		}
		if (n_written > 0)
			buffer->shrink(n_written);
	}
	return true;
}

bool VasHelper::Socket::isOK(int fd) {
	int result;
	socklen_t result_len = sizeof(result);
	return ((getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len) >= 0)
			&& (result == 0));
}

////////////////////////////////////////////////////////////

void VasHelper::Application::daemonize() {
	pid_t pid;
	if ((pid = fork()) < 0)
		return;
	else if (pid != 0)
		exit(0);
	setsid();
	return;
}

bool VasHelper::Application::setSignalHandler(int signal, void (*action)(int)) {
	struct sigaction sa;
	sa.sa_handler = action;
	sa.sa_flags = 0;
	if (sigemptyset(&sa.sa_mask) == -1 || sigaction(signal, &sa, NULL) == -1)
		return false;
	return true;
}

////////////////////////////////////////////////////////////
void VasHelper::Logger::debug(const char* file, int line, const char* fmt, ...) {
	time_t time_now = time(NULL);
	struct tm *tm_now = localtime((const time_t*) &time_now);
	fprintf(stdout, "[%04d-%02d-%02d %02d:%02d:%02d][DEBUG][%s:%d]",
			1900 + tm_now->tm_year, 1 + tm_now->tm_mon, tm_now->tm_mday,
			tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, file, line);
	va_list list;
	va_start(list, fmt);
	vfprintf(stdout, fmt, list);
	fprintf(stdout, "\n");
	va_end(list);
}

void VasHelper::Logger::error(const char* file, int line, const char* fmt, ...) {
	time_t time_now = time(NULL);
	struct tm *tm_now = localtime((const time_t*) &time_now);
	fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d][ERROR][%s:%d]",
			1900 + tm_now->tm_year, 1 + tm_now->tm_mon, tm_now->tm_mday,
			tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, file, line);
	va_list list;
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	fprintf(stderr, "\n");
	va_end(list);
}

////////////////////////////////////////////////////////////////

map<std::string, int> VasHelper::Config::_map2Integer;
map<std::string, std::string> VasHelper::Config::_map2String;
map<std::string, bool> VasHelper::Config::_map2Boolean;
map<std::string, double> VasHelper::Config::_map2Double;
void VasHelper::Config::load(const char* configFile) {
	ifstream iss(configFile, ifstream::binary);
	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(iss, root, false))
		throw VasException("cannot parse configure file " + (string)configFile);
	string prefix = "";
	VasHelper::Config::_travel(prefix, root);
	iss.close();
}

int VasHelper::Config::valueAsInteger(const string& key) {
	map<string, int>::const_iterator iter = VasHelper::Config::_map2Integer.find(key);
	if (iter == VasHelper::Config::_map2Integer.end())
		throw VasException("cannot find configure key = " + key);
	return iter->second;
}

const string& VasHelper::Config::valueAsString(const string& key) {
	map<string, string>::const_iterator iter = VasHelper::Config::_map2String.find(key);
	if (iter == VasHelper::Config::_map2String.end())
		throw VasException("cannot find configure key = " + key);
	return iter->second;
}

double VasHelper::Config::valueAsDouble(const string& key) {
	map<string, double>::const_iterator iter = VasHelper::Config::_map2Double.find(key);
	if (iter == VasHelper::Config::_map2Double.end())
		throw VasException("cannot find configure key = " + key);
	return iter->second;
}

bool VasHelper::Config::valueAsBoolean(const string& key) {
	map<string, bool>::const_iterator iter = VasHelper::Config::_map2Boolean.find(key);
	if (iter == VasHelper::Config::_map2Boolean.end())
		throw VasException("cannot find configure key = " + key);
	return iter->second;
}

void VasHelper::Config::_travel(string prefix, Json::Value& root) {
	if (root.size() > 0) {
		for (Json::ValueIterator itr = root.begin(); itr != root.end(); itr++) {
			string subPrefix = prefix;
			if (prefix.length() > 0)
				subPrefix += ".";
			subPrefix += itr.key().asString();
			VasHelper::Config::_travel(subPrefix, *itr);
		}
	} else {
		VasHelper::Config::_add(prefix, root);
	}
}

void VasHelper::Config::_add(const string& key, Json::Value val) {
	if (val.isString()) {
		VasHelper::Config::_map2String[key] = val.asString();
	} else if (val.isBool()) {
		VasHelper::Config::_map2Boolean[key] = val.asBool();
	} else if (val.isInt()) {
		VasHelper::Config::_map2Integer[key] = val.asInt();
	} else if (val.isUInt()) {
		VasHelper::Config::_map2Integer[key] = val.asUInt();
	} else if (val.isDouble()) {
		VasHelper::Config::_map2Double[key] = val.asDouble();
	}
}

////////////////////////////////////////////////////////////////

time_t VasHelper::Clock::_current = time(NULL);
time_t VasHelper::Clock::_recent = VasHelper::Clock::_current;
time_t VasHelper::Clock::sync() {
	_current = time(NULL);
	time_t delta = _current - _recent;
	_recent = _current;
	return delta;
}
time_t VasHelper::Clock::now() {
	return _current;
}
