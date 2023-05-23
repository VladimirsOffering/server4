#include "Logger.h"

void Logger::log(string msg)
{
	cout << "[Info]\t" << msg << endl;
}

void Logger::errorLog(string msg)
{
	cout << "[Error]\t" << msg << endl;
}

void Logger::logGame(string msg)
{
	cout << "[Game]\t" << msg << endl;
}
