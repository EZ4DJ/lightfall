#pragma once

#include <string>
#include "db/local_db.h"

namespace lightfall
{
	class Context
	{
		public:
			void initConfig();

			~Context();

			int getLocalMode();
			std::string getApiKey();
			std::string getSavePath();
			int getGameVer();

			LocalDB localDB;

		private:
			int m_localMode;
			std::string m_apiKey;
			std::string m_savePath;
			int m_gameVer;
	};

	extern Context context;
}


