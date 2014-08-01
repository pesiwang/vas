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

bool CHelper::Socket::read(int fd, CBuffer* buffer)
{
	char buf[1024];
	int curr_read = 0;
	int total_read = 0;
	while((curr_read = ::read(fd, buf, 1024)) > 0){
		buffer->write(buf, curr_read);
		total_read += curr_read;
	}

	if((curr_read == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
		return true;

	if((curr_read == -1) || (total_read == 0)){
		return false;
	}
	return true;
}

bool CHelper::Socket::write(int fd, CBuffer* buffer)
{
	bool blocked = false;
	char buf[1024];
	int curr_read = 0;
	while(!blocked && ((curr_read = buffer->peek(buf, 1024)) > 0)){
		int n_written = 0;
		int n_left = curr_read;
		while(n_left > 0){
			int written = ::write(fd, buf + n_written, n_left);
			if (written < n_left){
				if((written == -1) && (errno != EAGAIN) && (errno != EWOULDBLOCK))
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
//	if(setrlimit(RLIMIT_NOFILE, &resource) < 0)
//		return false;
	return true;
}
