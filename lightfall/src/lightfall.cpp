// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include <Windows.h>
#include "hook/score_hook.h"
#include "context.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: 
	{
		if (lightfall::context.initConfig() != 0)
		{
			break;
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

