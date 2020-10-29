#include "Logger.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>

Logger::Logger():
out(&std::cout)
{
	std::ios::sync_with_stdio(false);
}

std::string Logger::format(const char* s)
{
	while (*s)
	{
		if (*s == '%')
		{
			if (*(s+1) == '%')
			{
				++s;
			}
			else
			{
				throw std::runtime_error("missing argument in format string");
			}
		}

		ss << *s++;
	}

	auto str = ss.str();
	ss.str("");

	return str;
}

Logger& Logger::get()
{
	static Logger INSTANCE;
	return INSTANCE;
}

Logger::~Logger()
{
}
