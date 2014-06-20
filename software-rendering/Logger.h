#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>

#define LOCAL_BUF_SIZE 1024

class Logger
{
public:
	~Logger(void);
    
    static inline Logger& Instance(void)
    {
        static Logger s_inst;
        return s_inst;
    }

    void set_log_file(std::string file);

    static inline void GtLog(const char* format, ...)
    {
        Logger &logger = Logger::Instance();

        va_list args;
        va_start(args,format);
        char buffer[LOCAL_BUF_SIZE];
        strcpy_s(buffer, LOCAL_BUF_SIZE, "#0");
        strcat_s(buffer, LOCAL_BUF_SIZE, format);
        logger.Log( buffer, args );
        va_end(args);

    }

    static inline void GtLogInfo(const char* format, ...)
    {
        Logger &logger = Logger::Instance();

        va_list args;
        va_start(args,format);
        char buffer[LOCAL_BUF_SIZE];
        strcpy_s(buffer, LOCAL_BUF_SIZE, "#1");
        strcat_s(buffer, LOCAL_BUF_SIZE, format);
        logger.Log( buffer, args );
        va_end(args);
    }

    static inline void GtLogWarning(const char* format, ...)
    {
        Logger &logger = Logger::Instance();

        va_list args;
        va_start(args,format);
        char buffer[LOCAL_BUF_SIZE];
        strcpy_s(buffer, LOCAL_BUF_SIZE, "#2");
        strcat_s(buffer, LOCAL_BUF_SIZE, format);
        logger.Log( buffer, args );
        va_end(args);
    }

    static inline void GtLogError(const char* format, ...)
    {
        Logger &logger = Logger::Instance();

        va_list args;
        va_start(args,format);
        char buffer[LOCAL_BUF_SIZE];
        strcpy_s(buffer, LOCAL_BUF_SIZE, "#3");
        strcat_s(buffer, LOCAL_BUF_SIZE, format);
        logger.Log( buffer, args );
        va_end(args);
    }
private:
    Logger(void);
    Logger(const Logger&);
    Logger& operator=(const Logger&);

    void Log(const char* line);
    void Log(const char* format, va_list args);

	void FlushToFile();
	char *data_;
	size_t size_;
    std::string log_file_;
};

#undef LOCAL_BUF_SIZE
