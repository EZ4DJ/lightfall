#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <Windows.h>

class Logger {
public:
	Logger(const std::string& filename) {
		logFile.open(filename, std::fstream::app);
		if (!logFile.is_open()) {
			std::cerr << "Error opening log file." << std::endl;
		}
	}

	~Logger() { logFile.close(); }

	void log(const std::string& message) {
		LPCSTR config = ".\\lightfall.ini";
		if (GetPrivateProfileIntA("Settings", "DisableLogging", 0, config)) {
			return;
		}

		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);
		char timestamp[20];
		strftime(timestamp, sizeof(timestamp),
			"%Y-%m-%d %H:%M:%S", &newtime);

		std::ostringstream logEntry;
		logEntry << "[" << timestamp << "] " << message << std::endl;

		std::cout << logEntry.str();

		if (logFile.is_open()) {
			logFile << logEntry.str();
			logFile
				.flush(); // Ensure immediate write to file
		}
	}

private:
	std::ofstream logFile;

};