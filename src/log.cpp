#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdarg.h>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

#include "log_config.h"
#include "log.h"

//log context
const int MAX_SINGLE_LOG_SIZE = 2048;
const int ONE_DAY_SECONDS = 865400;  // 24 * 3600

int log_level = DEBUG_LEVEL;  //default log_level
std::string g_dir;
std::string g_config_file;
bool use_file_appender = false;
file_appender g_file_appender;

file_appender::file_appender() 
	: is_inited_(false), retain_day_(-1), last_sec_(0)
{
}

file_appender::~file_appender()
{
	if(fs_.is_open())
		fs_.close();
	if(is_inited_)
		pthread_mutex_destroy(&writelock_);
}

int file_appender::init(std::string& dir, std::string& log_file)
{
	//try to open the dir, and if failed, use the current "." dir
	if(!dir.empty()){
		int ret = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //see baidu baike "mkdir"
		if(ret != 0 && errno != EEXIST){
			printf("mkdir error which dir:%s err:%s\n", dir.c_str(), strerror(errno));
			is_inited_ = true;
			return -1;
		}
	}
	else{
		dir = "."; 
	}

	log_dir_ = dir;
	log_file_ = log_file;
	log_file_path_ = dir + "/" + log_file;
	fs_.open(log_file_path_.c_str(), std::fstream::out | std::fstream::app);  //append
	is_inited_ = true;
	pthread_mutex_init(&writelock_, NULL);
	return 0;
}

inline bool file_appender::is_inited()
{
	return is_inited_;
}

inline void file_appender::set_retain_day(int rt_day)
{
	retain_day_ = rt_day;
}

int file_appender::write_log(char* log, const char* format, va_list ap)
{
	pthread_mutex_lock(&writelock_);
	if(fs_.is_open()){
		vsnprintf(log, MAX_SINGLE_LOG_SIZE-1, format, ap);
		fs_<<log<<"\n";
		fs_.flush();
	}
	pthread_mutex_unlock(&writelock_);
	return 0;
}

int _get_log_level(const char* level_str)
{
	if(strcasecmp(level_str, "ERROR") == 0)
		return ERROR_LEVEL;
	else if(strcasecmp(level_str, "WARN") == 0)
		return WARN_LEVEL;
	else if(strcasecmp(level_str, "INFO") == 0)
		return INFO_LEVEL;
	else if(strcasecmp(level_str, "DEBUG") == 0)
		return DEBUG_LEVEL;
	return DEBUG_LEVEL;  //
}

inline void set_log_level(const char* level)
{
	log_level = _get_log_level(level);
}

//check need start log module from config file
int _check_config_file() 
{
	std::map<std::string, std::string> configs;
	std::string log_config_file = g_dir + "/" + g_config_file;
	
	get_config_map(log_config_file.c_str(), configs);
	if(configs.empty())
		return 0;
	
	//read log level
	std::string log_level_str = configs["log_level"];
	set_log_level(log_level_str.c_str());

	std::string rt_day = configs["retai_day"];
	if(!rt_day.empty())
		g_file_appender.set_retain_day(atoi(rt_day.c_str()));
	
	//read log file
	std::string dir = configs["log_dir"];
	std::string log_file = configs["log_file"];

	int ret = 0;
	if(!log_file.empty()){  //if log_file not empty, start the file appender
		use_file_appender = true;
		if(!g_file_appender.is_inited()) {
			ret = g_file_appender.init(dir, log_file);
		}
	}
	return ret;
}

//log init function, invoke by main function
int log_init(std::string dir, std::string file)
{
	g_dir = dir;
	g_config_file = file;
	return _check_config_file();
}

std::string _get_show_time(timeval tv)
{
	char show_time[40];
	memset(show_time, 0, sizeof(show_time));

	struct tm* tm;
	tm = localtime(&tv.tv_sec);

	sprintf(show_time, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 
			tm->tm_year + 1900, tm->tm_mon+1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec/1000));
	return std::string(show_time);
}

void _log(const char* format, va_list ap)
{
	if(!use_file_appender){   //if no config, send log to stdout
		vprintf(format, ap);
		printf("\n");
		return ;
	}

	struct timeval now;
	struct timezone tz;
	gettimeofday(&now, &tz);
	std::string final_format = _get_show_time(now) + " " + format;

	//g_file_appender.shift_file_if_need(now, tz);
	char single_log[MAX_SINGLE_LOG_SIZE];
	memset(single_log, 0, MAX_SINGLE_LOG_SIZE);
	g_file_appender.write_log(single_log, final_format.c_str(), ap);
}

void log_error(const char* format, ...)
{
	if(log_level < ERROR_LEVEL)
		return ;
	
	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_warn(const char* format, ...)
{
	if(log_level < WARN_LEVEL)
		return ;
	
	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_info(const char* format, ...)
{
	if(log_level < INFO_LEVEL)
		return ;
	
	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}

void log_debug(const char* format, ...)
{
	if(log_level < DEBUG_LEVEL)
		return ;

	va_list ap;
	va_start(ap, format);

	_log(format, ap);

	va_end(ap);
}
