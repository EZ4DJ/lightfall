#include "network.h"

#include "json.hpp"
#include "logger.h"
#include "context.h"
#include "ez2ac.h"

using ez2ac::scoredata;

namespace lightfall
{
	void Network::initNetwork()
	{
		URL_COMPONENTS components;
		SecureZeroMemory(&components, sizeof(components));
		components.dwStructSize = sizeof(components);
		components.dwSchemeLength = static_cast<DWORD>(-1);
		components.dwHostNameLength = static_cast<DWORD>(-1);
		components.dwUrlPathLength = static_cast<DWORD>(-1);
		components.dwExtraInfoLength = static_cast<DWORD>(-1);

		if (!WinHttpCrackUrl(context.getURL().c_str(), 
							 static_cast<DWORD>(wcslen(context.getURL().c_str())),
							 0, &components))
		{
			log("Failed to crack URL: error %u", GetLastError());
			return;
		}

		std::wstring hostname{ components.lpszHostName, components.dwHostNameLength };
		std::wstring path{ components.lpszUrlPath, components.dwUrlPathLength };

		session = WinHttpOpen(L"lightfall", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
							  WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

		if (!session)
		{
			log("Failed to open WinHTTP session: error %u", GetLastError());
			return;
		}

		connection = WinHttpConnect(session, hostname.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);

		if (!connection)
		{
			log("Failed to open WinHTTP connection: error %u", GetLastError());
			return;
		}

		request = WinHttpOpenRequest(connection, L"POST", path.c_str(), nullptr, 
									 WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

		if (!request)
		{
			log("Failed to open WinHTTP request: error %u", GetLastError());
			return;
		}
	}

	Network::~Network()
	{
		if (request) WinHttpCloseHandle(request);
		if (connection) WinHttpCloseHandle(connection);
		if (session) WinHttpCloseHandle(session);
	}

	void Network::submitScore()
	{
		nlohmann::json msg;

		msg["title"] = scoredata.title;
		msg["mode"] = scoredata.mode;
		msg["difficulty"] = scoredata.difficulty;
		msg["level"] = scoredata.level;
		msg["score"] = scoredata.score;
		msg["rate"] = scoredata.rate;
		msg["grade"] = scoredata.grade;

		msg["total_notes"] = scoredata.total_notes;
		msg["kool"] = scoredata.kool;
		msg["cool"] = scoredata.cool;
		msg["good"] = scoredata.good;
		msg["miss"] = scoredata.miss;
		msg["fail"] = scoredata.fail;
		msg["max_combo"] = scoredata.max_combo;

		msg["random"] = scoredata.random_op;
		msg["auto"] = scoredata.auto_op;
		msg["stage"] = scoredata.stage;

		log("Submitting score for song %s...", scoredata.title);
	}
}