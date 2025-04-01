// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include <Windows.h>
#include "context.h"
#include "hook/score_hook.h"
#include "util/logger.h"

using lightfall::context;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: 
	{
		context.initConfig();

		log("Starting up...");

		if (context.getGameVer() != 21)
		{
			log("Game version not Final: EX, stopping!");
			break;
		}

		if (context.getLocalMode())
		{
			log("Starting in local mode, opening database...");

			if (context.localDB.initDB(context.getSavePath()))
			{
				break;
			}
		}

		lightfall::installHook();

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

