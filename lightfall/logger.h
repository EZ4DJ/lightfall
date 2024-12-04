#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

class Logger {
public:
	// Constructor: Opens the log file in append mode
	Logger(const std::string& filename)
	{
		logFile.open(filename, std::fstream::app);
		if (!logFile.is_open()) {
			std::cerr << "Error opening log file." << std::endl;
		}
	}

	// Destructor: Closes the log file
	~Logger() { logFile.close(); }

	// Logs a message with a given log level
	void log(const std::string& message)
	{
		// Get current timestamp
		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);
		char timestamp[20];
		strftime(timestamp, sizeof(timestamp),
			"%Y-%m-%d %H:%M:%S", &newtime);

		// Create log entry
		std::ostringstream logEntry;
		logEntry << "[" << timestamp << "] " << message << std::endl;

		// Output to console
		std::cout << logEntry.str();

		// Output to log file
		if (logFile.is_open()) {
			logFile << logEntry.str();
			logFile
				.flush(); // Ensure immediate write to file
		}
	}

private:
	std::ofstream logFile; // File stream for the log file

};