/*
 * VasHelper.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#ifndef INCLUDE_CORE_VAS_HELPER_H_
#define INCLUDE_CORE_VAS_HELPER_H_

#include <time.h>
#include <json/json.h>
#include "core/VasBuffer.h"
#define VAS_LOGGER_DEBUG(...) VasHelper::Logger::debug(__FILE__, __LINE__, __VA_ARGS__);
#define VAS_LOGGER_ERROR(...) VasHelper::Logger::error(__FILE__, __LINE__, __VA_ARGS__);

class VasHelper {
public:
	class Socket {
	public:
		static int create();
		static int listen(const char* addr, unsigned short port);
		static int connect(const char* addr, unsigned short port);
		static int accept(int listenerFd);
		static bool read(int fd, VasBuffer* buffer);
		static bool write(int fd, VasBuffer* buffer);
		static bool isOK(int fd);

	};

	class Application {
	public:
		static void daemonize();
		static bool setSignalHandler(int signal, void (*action)(int));
	};

	class Logger {
	public:
		static void debug(const char* file, int line, const char* fmt, ...);
		static void error(const char* file, int line, const char* fmt, ...);
	};

	class Config {
	public:
		static void load(const char* configFile);
		static int valueAsInteger(const std::string& key);
		static const std::string& valueAsString(const std::string& key);
		static double valueAsDouble(const std::string& key);
		static bool valueAsBoolean(const std::string& key);
	private:
		static std::map<std::string, int> _map2Integer;
		static std::map<std::string, std::string> _map2String;
		static std::map<std::string, bool> _map2Boolean;
		static std::map<std::string, double> _map2Double;
	private:
		static void _travel(std::string prefix, Json::Value& root);
		static void _add(const std::string& key, Json::Value val);
	};

	class Clock {
	public:
		static time_t sync();
		static time_t now();
	private:
		static time_t _current;
		static time_t _recent;
	};
};
#endif /* INCLUDE_CORE_VAS_HELPER_H_ */
