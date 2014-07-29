#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config.h"
#include "definition.h"

CConfig* CConfig::_instance = NULL;
const char* CConfig::_pathname = NULL;

static void _ini_parse_file(FILE* file, void (*handler)(void*, const char*, const char*, const char*), void* context);

static void _config_ini_handler(void* context, const char* section, const char* name, const char* value){
	CConfig* config = (CConfig*)context;
	#define MATCH(s, n) strcasecmp(section, s) == 0 && strcasecmp(name, n) == 0
	if(MATCH("NETWORK", "host"))
		strcpy(config->network.host, value);
	else if(MATCH("NETWORK", "port"))
		config->network.port = (unsigned short)(atoi(value));
    else if(MATCH("SERVER", "max_client"))
		config->server.maxClients = atoi(value);
	else if(MATCH("SERVER", "max_idle_time"))
		config->server.maxIdleTime = atoi(value);
	else if(MATCH("SERVER", "daemonize"))
		config->server.daemonize = (atoi(value) == 0) ? false : true;
}

CConfig* CConfig::instance()
{
	if(CConfig::_instance == NULL)
		CConfig::_instance = new CConfig();
	return CConfig::_instance;
}

void CConfig::release()
{
	if(NULL != CConfig::_instance){
		delete CConfig::_instance;
		CConfig::_instance = NULL;
	}
}

void CConfig::setPathName(const char* pathname)
{
	CConfig::_pathname = pathname;
}

//////////////////////////////////////////
CConfig::CConfig()
{
	//step 1. init with default value
	this->server.maxIdleTime = 60;
	this->server.maxClients = 1000;
	this->server.daemonize = false;
	
	strcpy(this->network.host, "0.0.0.0");
	this->network.port = 8211;

	//step 2. load from inifile
	if(NULL != CConfig::_pathname){
		FILE* stream = fopen(CConfig::_pathname, "r");
		if(NULL == stream)
			throw VAS_ERR_INTERNAL;

		_ini_parse_file(stream, _config_ini_handler, this);
		fclose(stream);
	}
}

CConfig::~CConfig()
{
}

//------------------------ini parser portion----------------------------//
static char* _rstrip(char* s){
	char* p = s + strlen(s);
	while (p > s && isspace((unsigned char)(*--p)))
		*p = '\0';
	return s;
}

static char* _lskip(const char* s){
	while (*s && isspace((unsigned char)(*s)))
		s++;
	return (char*)s;
}

static char* _find_char_or_comment(const char* s, char c){
	int was_whitespace = 0;
	while (*s && *s != c && !(was_whitespace && *s == ';')) {
		was_whitespace = isspace((unsigned char)(*s));
		s++;
	}
	return (char*)s;
}

static inline char* _strncpy0(char* dest, const char* src, size_t size){
	strncpy(dest, src, size);
	dest[size - 1] = '\0';
	return dest;
}

static void _ini_parse_file(FILE* file, void (*handler)(void*, const char*, const char*, const char*), void* context){
	char* line;
	char section[500] = "";
	char prev_name[500] = "";

	char* start;
	char* end;
	char* name;
	char* value;
	int lineno = 0;
	int error = 0;

	line = (char*)malloc(1024);
	while (fgets(line, 1024, file) != NULL) {
		lineno++;
		start = line;
		if (lineno == 1 && (unsigned char)start[0] == 0xEF &&
						   (unsigned char)start[1] == 0xBB &&
						   (unsigned char)start[2] == 0xBF) {
			start += 3;
		}
		start = _lskip(_rstrip(start));

		if (*start == ';' || *start == '#') {
			/* Per Python ConfigParser, allow '#' comments at start of line */
		}
		else if (*prev_name && *start && start > line) {
			/* Non-black line with leading whitespace, treat as continuation
			   of previous name's value (as per Python ConfigParser). */
			handler(context, section, prev_name, start);
			if (!error)
				error = lineno;
		}
		else if (*start == '[') {
			/* A "[section]" line */
			end = _find_char_or_comment(start + 1, ']');
			if (*end == ']') {
				*end = '\0';
				_strncpy0(section, start + 1, sizeof(section));
				*prev_name = '\0';
			}
			else if (!error) {
				/* No ']' found on section line */
				error = lineno;
			}
		}
		else if (*start && *start != ';') {
			/* Not a comment, must be a name[=:]value pair */
			end = _find_char_or_comment(start, '=');
			if (*end != '=') {
				end = _find_char_or_comment(start, ':');
			}
			if (*end == '=' || *end == ':') {
				*end = '\0';
				name = _rstrip(start);
				value = _lskip(end + 1);
				end = _find_char_or_comment(value, '\0');
				if (*end == ';')
					*end = '\0';
				_rstrip(value);

				/* Valid name[=:]value pair found, call handler */
				_strncpy0(prev_name, name, sizeof(prev_name));
				handler(context, section, name, value);
				if (!error)
					error = lineno;
			}
			else if (!error) {
				/* No '=' or ':' found on name[=:]value line */
				error = lineno;
			}
		}
	}

	free(line);
	return;
}
