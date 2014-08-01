#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "helper.h"

int CHelper::Socket::listen(const char* addr, unsigned short port)
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	int listenerFd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenerFd < 0)
		return -1;

	int optval = 1;
	setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval, sizeof(optval));

	fcntl(listenerFd, F_SETFL, fcntl(listenerFd, F_GETFL) | O_NONBLOCK);

	if((::bind(listenerFd, (struct sockaddr *)&sin, sizeof(sin)) < 0) || (::listen(listenerFd, SOMAXCONN) < 0)){
		close(listenerFd);
		return -1;
	}
	return listenerFd;
}

int CHelper::Socket::connect(const char* addr, unsigned short port)
{
	int clientFd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientFd < 0)
		return -1;

	fcntl(clientFd, F_SETFL, fcntl(clientFd, F_GETFL) | O_NONBLOCK);

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	int res = ::connect(clientFd, (struct sockaddr *)&sin, sizeof(sin));
	if ((res < 0) && (errno != EINPROGRESS)){
		close(clientFd);
		return -1;
	}
	return clientFd;
}

int CHelper::Socket::accept(int listenerFd)
{
	int sin_len = sizeof(struct sockaddr);
	struct sockaddr_in sin;
	int clientFd = ::accept(listenerFd, (struct sockaddr *)&sin, (socklen_t *)&sin_len);
	if(clientFd < 0)
		return -1;

	fcntl(clientFd, F_SETFL, fcntl(clientFd, F_GETFL) | O_NONBLOCK);
	return clientFd;
}

//////////////////////////////////////////////////////////////////

void CHelper::Application::daemonize()
{
	pid_t pid;
	if((pid = fork()) < 0)
		return;
	else if(pid != 0)
		exit(0);

	setsid();
	return;
}

bool CHelper::Application::setFileLimit(int limit)
{
	struct rlimit resource;
	resource.rlim_cur = limit;
	resource.rlim_max = limit;
	if(setrlimit(RLIMIT_NOFILE, &resource) < 0)
		return false;
	return true;
}
