// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include <iostream>
#include <fstream>
#include <thread>
#include <Windows.h>
#include "include\\MinHook.h"

#pragma comment(lib, "libMinHook.x86.lib")

uintptr_t baseAddress;
LPCSTR config = ".\\lightfall.ini";
LPCSTR twoezconfig = ".\\2EZ.ini";

DWORD LightfallThread() {
	int gameVer = GetPrivateProfileIntA("Settings", "GameVer", 0, twoezconfig);
	if (gameVer != 21) {
		printf("Game version not Final: EX, stopping\n");
		return NULL;
	}

	baseAddress = (uintptr_t)GetModuleHandleA(NULL);

	//Short sleep to fix crash when using legitimate data with dongle, can be overrden in ini if causing issues.
	Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoezconfig));

	std::ofstream testfile("h.txt");
	testfile << "hi";
	testfile.close();

	return NULL;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LightfallThread, 0, 0, 0);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

