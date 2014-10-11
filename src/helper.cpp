/*
 * helper.cpp
 *
 *  Created on: Oct 8, 2014
 *	  Author: chenrui
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include "helper.h"

using namespace vas;

int Helper::Socket::create(){
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
		return -1;

	if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) < 0){
		::close(fd);
		return -1;
	}
	return fd;
}


int Helper::Socket::accept(int listenerFd){
	int sin_len = sizeof(struct sockaddr);
	struct sockaddr_in sin;
	int clientFd = ::accept(listenerFd, (struct sockaddr *)&sin, (socklen_t *)&sin_len);
	if(clientFd < 0)
		return -1;

	if(fcntl(clientFd, F_SETFL, fcntl(clientFd, F_GETFL) | O_NONBLOCK) < 0){
		close(clientFd);
		return -1;
	}
	return clientFd;
}

int Helper::Socket::listen(const char* addr, unsigned short port){
	int fd = Helper::Socket::create();
	if(fd < 0)
		return -1;

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	int optval = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval, sizeof(optval));

	if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) < 0){
		close(fd);
		return -1;
	}

	if((::bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) || (::listen(fd, SOMAXCONN) < 0)){
		close(fd);
		return -1;
	}
	return fd;
}

int Helper::Socket::connect(const char* addr, unsigned short port){
	int fd = Helper::Socket::create();
	if(fd < 0)
		return -1;

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	int res = ::connect(fd, (struct sockaddr *)&sin, sizeof(sin));
	if ((res < 0) && (errno != EINPROGRESS)){
		::close(fd);
		return -1;
	}
	return fd;
}

bool Helper::Socket::read(int fd, Buffer* buffer){
	char buf[1024];
	int curr_read = 0;
	int total_read = 0;
	while((curr_read = ::read(fd, buf, 1024)) > 0){
		buffer->write(buf, curr_read);
		total_read += curr_read;
	}

	if((curr_read == -1) && (errno == EAGAIN))
		return true;

	if((curr_read == -1) || (total_read == 0)){
		return false;
	}
	return true;
}

bool Helper::Socket::write(int fd, Buffer* buffer){
	bool blocked = false;
	while(!blocked){
		int curr_read = buffer->size() > 1024 ? 1024 : buffer->size();
		if(curr_read <= 0)
			break;

		char buf[1024];
		buffer->peek(buf, curr_read);

		int n_written = 0;
		int n_left = curr_read;
		while(n_left > 0){
			int written = ::write(fd, buf + n_written, n_left);
			if (written < n_left){
				if((written == -1) && (errno != EAGAIN))
					return false;

				if(written == -1){
					blocked = true;
					break;
				}
			}

			if(written > 0){
				n_written += written;
				n_left -= written;
			}
		}

		if(n_written > 0)
			buffer->shrink(n_written);
	}
	return true;
}

bool Helper::Socket::isOK(int fd){
	int result;
	socklen_t result_len = sizeof(result);
	return ((getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len) >= 0) && (result == 0));
}

////////////////////////////////////////////////////////////

void Helper::Application::daemonize()
{
    pid_t pid;
    if((pid = fork()) < 0)
        return;
    else if(pid != 0)
        exit(0);

    setsid();
    return;
}

bool Helper::Application::setSignalHandler(int signal, void (*action)(int))
{
    struct sigaction sa;
    sa.sa_handler = action;
    sa.sa_flags = 0;
    if(sigemptyset(&sa.sa_mask) == -1 || sigaction(signal, &sa, NULL) == -1)
        return false;
    return true;
}

////////////////////////////////////////////////////////////
void Helper::Logger::debug(const char* file, int line, const char* fmt, ...){
	time_t time_now = time(NULL);
	struct tm *tm_now = localtime((const time_t*)&time_now);
	fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d][DEBUG][%s:%d]", 1900 + tm_now->tm_year, 1 + tm_now->tm_mon, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec , file, line);
	va_list list;
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	fprintf(stderr, "\n");
	va_end(list);
}

void Helper::Logger::error(const char* file, int line, const char* fmt, ...){
	time_t time_now = time(NULL);
	struct tm *tm_now = localtime((const time_t*)&time_now);
	fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d][ERRIR][%s:%d]", 1900 + tm_now->tm_year, 1 + tm_now->tm_mon, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec , file, line);
	va_list list;
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	fprintf(stderr, "\n");
	va_end(list);
}

////////////////////////////////////////////////////////////////
time_t Helper::Clock::_current = time(NULL);
time_t Helper::Clock::_recent = Helper::Clock::_current;

time_t Helper::Clock::sync(){
	_current = time(NULL);
	time_t delta = _current - _recent;
	_recent = _current;
	return delta;
}

time_t Helper::Clock::now(){
	_current = time(NULL);
	return _current;
}
