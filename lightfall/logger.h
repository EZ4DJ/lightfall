#pragma once

#include <string>
#include <ctime>

#define log(format, ...) Logger::write(format, ## __VA_ARGS__)

class Logger
{
	public:
		static inline std::string getTimestamp()
		{
			struct tm newtime;
			time_t now = time(0);
			localtime_s(&newtime, &now);

			char timestamp[23];

			strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S] ", &newtime);

			return std::string(timestamp);
		}

		template <typename... Args> static void write(const std::string& format, Args... args)
		{
			auto message = std::string(getTimestamp() + format + "\n");

			printf_s(message.c_str(), args...);
		}

};