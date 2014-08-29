#ifndef _VAS_EVENT_BASE_H
#define _VAS_EVENT_BASE_H

#include <map>
#include <list>
#include <time.h>
#include <stdio.h>
#include "buffer.h"
#include "helper.h"

#define _log_printf(ls, fn, ln, ... ) do {time_t time_now; time_now = time(&time_now); struct tm *tm_now = localtime((const time_t*)&time_now); fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d][%s][%s:%d]", 1900 + tm_now->tm_year, 1 + tm_now->tm_mon, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec , ls, fn, ln); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }while(0)

#define log_error( ... ) _log_printf("ERROR", __FILE__, __LINE__, __VA_ARGS__ )
#ifdef DEBUG
#define log_debug( ... ) _log_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__ )
#else
#define log_debug( ... )
#endif

extern time_t g_timeNow;

namespace vas
{
	enum VAS_STATUS
	{
		VAS_STATUS_STOPPED = 0,
		VAS_STATUS_RUNNING,
		VAS_STATUS_STOPPING
	};

	class EventBase
	{
		public:
			//observer socket representant
			class Observer
			{
				public:
					Observer() {}
					virtual ~Observer() {}

					virtual void onStarted() = 0;
					virtual void onStopped() = 0;
					virtual void onTimer() = 0;
			};

			//listener socket representant
			class Listener
			{
				public:
					Listener() : fd(-1) {}
					virtual ~Listener() {}

					virtual void onAccepted(int clientFd) = 0;
					virtual void onClosed() = 0;

				public:
					int fd;
			};

			//handler socket representant
			class Handler
			{
				public:
					Handler() : fd(-1), input(NULL), output(NULL), connected(false), time(g_timeNow) { this->input = new Buffer(); this->output = new Buffer(); }
					virtual ~Handler() { if(NULL != this->input) { delete this->input; this->input = NULL; } if(NULL != this->output) { delete this->output; this->output = NULL; } }

					virtual void onConnected() = 0;
					virtual void onData() = 0;
					virtual void onClosed() = 0;

				public:
					int fd;
					Buffer* input;
					Buffer* output;
					bool connected;
					time_t time;
			};

		public:
			static EventBase* instance();
			static void stop(int sig);
			void dispatch();

			void add(Handler* handler);
			void add(Listener* listener);
			void add(Observer* observer);
			void remove(Handler* handler);
			void remove(Listener* listener);
			void remove(Observer* observer);
			void write(Handler* handler);

			Handler* getHandler(int fd);
			Listener* getListener(int fd);

		protected:
			EventBase();
			virtual ~EventBase();

			void _doTimer();

		protected:
			volatile VAS_STATUS _status;
			int _epollFd;

			std::map<int, Listener*> _listeners;
			std::map<int, Handler*> _handlers;
			std::list<Observer*> _observers;
	};
}

#endif
