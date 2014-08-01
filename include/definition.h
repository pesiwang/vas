#ifndef _VAS_DEFINITION_H
#define _VAS_DEFINITION_H

#include <time.h>
#include <stdio.h>

#define _log_printf(ls, fn, ln, ... ) do {time_t time_now; time_now = time(&time_now); struct tm *tm_now = localtime((const time_t*)&time_now); fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d][%s][%s:%d]", 1900 + tm_now->tm_year, 1 + tm_now->tm_mon, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec , ls, fn, ln); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }while(0)

#define log_error( ... ) _log_printf("ERROR", __FILE__, __LINE__, __VA_ARGS__ )
#ifdef DEBUG
#define log_debug( ... ) _log_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__ )
#else
#define log_debug( ... )
#endif

enum VAS_ERROR 
{
	VAS_ERR_INTERNAL = 0x0001
};

enum VAS_COMMAND
{
	VAS_COMMAND_ADD_FD = 0x01,
	VAS_COMMAND_DEL_FD,
	VAS_COMMAND_ADD_BUFFER
};

enum VAS_REASON
{
	VAS_REASON_CLIENT_CLOSED	= 0x01,
	VAS_REASON_SERVER_CLOSED,
	VAS_REASON_TIMEOUT
};
#endif
