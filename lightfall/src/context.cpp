#include "context.h"

#include <Windows.h>
#include <shlwapi.h>

namespace lightfall
{
	Context context;

	void Context::initConfig()
	{
		LPCSTR lightfallConfig = ".\\lightfall.ini";

		char savePath[MAX_PATH];
		GetPrivateProfileStringA("Settings", "SavePath", NULL, savePath, MAX_PATH, lightfallConfig);

		m_savePath = std::string(savePath);

		PathAppendA(savePath, "lightfall.log");
#pragma warning(suppress: 4996)
		freopen(savePath, "a", stdout);

		LPCSTR twoEzConfig = ".\\2EZ.ini";
		
		// Short sleep to fix crash when using legitimate data with dongle, can be overriden in 2EZ.ini if causing issues
		Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoEzConfig));

		m_gameVer = GetPrivateProfileIntA("Settings", "GameVer", 21, twoEzConfig);

		m_localMode = GetPrivateProfileIntA("Settings", "LocalMode", 1, lightfallConfig);

		char buff[64];
		GetPrivateProfileStringA("Settings", "ApiKey", NULL, buff, 64, lightfallConfig);

		m_apiKey = std::string(buff);
	}

	Context::~Context()
	{
		fclose(stdout);
	}

	int Context::getLocalMode()
	{
		return m_localMode;
	}

	std::string Context::getApiKey()
	{
		return m_apiKey;
	}

	std::string Context::getSavePath()
	{
		return m_savePath;
	}

	int Context::getGameVer()
	{
		return m_gameVer;
	}
}