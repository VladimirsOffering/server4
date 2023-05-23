#pragma once
#include <string>
#include <iostream>
#include <chrono>
using namespace std;

class Logger
{
	Logger()
	{
		setlocale(LC_ALL, "rus");
	}
public:
	static void log(string msg);
	static void errorLog(string msg);
	static void logGame(string msg);

};