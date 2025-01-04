#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

class Logger {
public:
	bool disabled = false;
	void open(const std::string& filename) {
		if (disabled) {
			return;
		}
		logFile.open(filename, std::fstream::app);
	}
	~Logger() {
		if (disabled) {
			return;
		}
		logFile.close();
	}
	void logTimestamp(const std::string& message) {
		if (disabled) {
			return;
		}
		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);
		char timestamp[20];
		strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &newtime);
		std::ostringstream logEntry;
		logEntry << "[" << timestamp << "] " << message;
		if (logFile.is_open()) {
			logFile << logEntry.str();
			logFile.flush(); // Ensure immediate write to file
		}
	}
	void log(const std::string& message) {
		if (disabled) {
			return;
		}
		if (logFile.is_open()) {
			logFile << message;
			logFile.flush();
		}
	}
private:
	std::ofstream logFile;
};