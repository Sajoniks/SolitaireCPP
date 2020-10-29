#pragma once
#include <iomanip>
#include <time.h>
#include <iosfwd>
#include <mutex>
#include <sstream>

class Logger
{
	Logger();
	std::ostream* out;

	std::mutex mutex;
	std::stringstream ss;
	std::stringstream logHistory;

	template<typename T, typename ...Ts>
	std::string format(const char* s, const T& first, Ts... args);
	std::string format(const char* s);
	
public:

	void bind(std::ostream& ios) { out = &ios; }
	static Logger& get();

	template <typename... Args>
	void log(const char* message,Args... args);

	~Logger();
};

template <typename T, typename ... Ts>
std::string Logger::format(const char* s, const T& first, Ts... args)
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
				ss << first;
				return format(s + 1, std::forward<Ts>(args)...);
			}
		}

		ss << *s++;
	}

	throw std::logic_error("extra arguments provided");
}

template <typename ... Args>
void Logger::log(const char* message, Args... args)
{
#if _DEBUG
	std::lock_guard guard(mutex);

	auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm time;
	localtime_s(&time, &now);

	auto formatStr = format(message, std::forward<Args>(args)...);
	auto formatTime = std::put_time(&time, "[%F][%T] ");
	
	*out << formatTime << formatStr << '\n';
	logHistory << formatTime << formatStr << '\n';
#endif
}
