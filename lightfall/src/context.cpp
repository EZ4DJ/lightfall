#include <Windows.h>
#include <shlwapi.h>
#include "context.h"
#include "util/logger.h"

namespace lightfall
{
	Context context;

	int Context::initConfig()
	{
		LPCSTR lightfallConfig = ".\\lightfall.ini";

		char savePath[MAX_PATH];
		GetPrivateProfileStringA("Settings", "SavePath", NULL, savePath, MAX_PATH, lightfallConfig);

		m_savePath = std::string(savePath);

		FILE* f;
		PathAppendA(savePath, "lightfall.log");
		freopen_s(&f, savePath, "a", stdout);

		log("Starting up...");

		LPCSTR twoEzConfig = ".\\2EZ.ini";
		
		// Short sleep to fix crash when using legitimate data with dongle, can be overriden in 2EZ.ini if causing issues.
		Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoEzConfig));

		m_gameVer = GetPrivateProfileIntA("Settings", "GameVer", 21, twoEzConfig);

		if (m_gameVer != 21)
		{
			log("Game version not Final: EX, stopping!");
			return 1;
		}

		m_localMode = GetPrivateProfileIntA("Settings", "LocalMode", 1, lightfallConfig);

		if (m_localMode)
		{
			log("Starting in local mode, opening database...");
			return localDB.initDB(m_savePath);
		}

		return 0;
	}

	int Context::getLocalMode()
	{
		return m_localMode;
	}

	std::string Context::getApiKey()
	{
		return std::string();
	}

	std::string Context::getSavePath()
	{
		return m_savePath;
	}

	uint8_t Context::getGameVer()
	{
		return m_gameVer;
	}
}