#pragma once

#include <cstdint>
#include <string>
#include "db/local_db.h"

namespace lightfall
{
	class Context
	{
		public:
			int initConfig();

			int getLocalMode();
			std::string getApiKey();
			std::string getSavePath();
			uint8_t getGameVer();

			LocalDB localDB;

		private:
			int m_localMode;
			std::string m_apiKey;
			std::string m_savePath;
			uint8_t m_gameVer;
	};

	extern Context context;
}


