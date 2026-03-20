#pragma once

#include <string>
#include "local_db.h"

namespace lightfall
{
	class Context
	{
		public:
			void initConfig();

			~Context();

			int getLocalMode();
			int getGameVer();

			std::string getSavePath();
			std::wstring getURL();
			std::wstring getApiKey();

			LocalDB localDB;

		private:
			int m_localMode;
			int m_gameVer;

			std::string m_savePath;
			std::wstring m_URL;
			std::wstring m_apiKey;
	};

	extern Context context;
}


