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
		freopen(savePath, "w", stdout);

		LPCSTR twoEzConfig = ".\\2EZ.ini";
		
		// Short sleep to fix crash when using legitimate data with dongle, can be overriden in 2EZ.ini if causing issues
		Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoEzConfig));

		m_gameVer = GetPrivateProfileIntA("Settings", "GameVer", 21, twoEzConfig);

		m_localMode = GetPrivateProfileIntA("Settings", "LocalMode", 1, lightfallConfig);

		wchar_t url[255];
		GetPrivateProfileStringW(L"Settings", L"URL", NULL, url, 255, L".\\lightfall.ini");

		m_URL = std::wstring(url);

		wchar_t apiKey[65];
		GetPrivateProfileStringW(L"Settings", L"apiKey", NULL, apiKey, 65, L".\\lightfall.ini");

		m_apiKey = std::wstring(apiKey);
	}

	Context::~Context()
	{
		fclose(stdout);
	}

	int Context::getLocalMode()
	{
		return m_localMode;
	}

	int Context::getGameVer()
	{
		return m_gameVer;
	}

	std::string Context::getSavePath()
	{
		return m_savePath;
	}

	std::wstring Context::getURL()
	{
		return m_URL;
	}

	std::wstring Context::getApiKey()
	{
		return m_apiKey;
	}
}