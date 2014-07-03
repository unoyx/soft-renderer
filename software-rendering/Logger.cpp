#include "Logger.h"

using std::string;

const int LOG_FILE_SZIE = 1024 * 50;

#pragma warning(push)
// 禁用unsafe function的警告
#pragma warning(disable:4996)

Logger::Logger(void)
    // 1mb
    :data_(new char[LOG_FILE_SZIE])
    ,size_(0)
{
	AllocConsole();

	(void)freopen("CONOUT$","w+t",stdout);  
	(void)freopen("CONIN$","r+t",stdin);
}

Logger::~Logger(void)
{
	FlushToFile();

    if (data_)
        delete[] data_;
	fclose(stdout);
	fclose(stdin);
	FreeConsole();
}

void Logger::Log( const char* line )
{
	size_t length = strlen(line);
	
	// 缓存装不下了，写入文件
	if (size_ + length + 2 > LOG_FILE_SZIE)
	{
		FlushToFile();
	}
	
	// 装入缓存
	{
		SYSTEMTIME time;
		GetSystemTime(&time);

		char* buffer = new char[length + 100];

		static HANDLE consolehwnd;
		consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);

		switch(line[1])
		{
		case '0':
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );
			break;
		case '1':
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN );
			break;
		case '2':
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY );
			break;
		case '3':
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED | FOREGROUND_INTENSITY );
			break;
		}

		sprintf(buffer, "[%d/%d/%d %d:%d(+8)] %s \r\n", time.wYear, time.wMonth, time.wDay, (time.wHour + 8) % 24, time.wMinute, line + 2);

		memcpy(data_ + size_, buffer, strlen(buffer));
		size_ += strlen(buffer);

		printf("%s", buffer);

		delete[] buffer;
	}
	
}

void Logger::Log( const char* format, va_list args )
{
	char buffer[512];
	vsprintf(buffer, format, args);

	Log(buffer);
}

void Logger::set_log_file(string file)
{
    log_file_ = file;
}

void Logger::FlushToFile()
{
    if (!log_file_.empty())
    {
        // write to file
        FILE* pFile = NULL;
        fopen_s(&pFile, log_file_.c_str(), "ab");
        fwrite(data_, 1, size_, pFile);
        fclose(pFile);
    }
    // remove size to 0
    size_ = 0;
}

#pragma warning(default:4996)
//  启用unsafe function的警告
#pragma warning(pop)