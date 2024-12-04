// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <Windows.h>
#include "MinHook.h"
#include "logger.h"

#pragma comment(lib, "MinHook.x86.lib")

uintptr_t baseAddress;
LPCSTR config = ".\\lightfall.ini";
Logger logger("lightfall.log");

void ReadMemory(void* dest, uintptr_t offset, size_t size) {
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(baseAddress + offset), size, PAGE_EXECUTE_READWRITE, &OldProtection);
	logger.log(std::to_string(OldProtection));
	CopyMemory(dest, (LPVOID)(baseAddress + offset), size);
	VirtualProtect((LPVOID)(baseAddress + offset), size, OldProtection, NULL);
}

typedef void(__fastcall showResultsScreen)(uintptr_t *input);
showResultsScreen* fpResultsScreen = NULL;

void __fastcall replacedShowResultsScreen(uintptr_t *input) {
	uintptr_t scoreDataOffset = baseAddress + 0x95C5C;
	uint32_t* scorep = reinterpret_cast<uint32_t*>(baseAddress + 0x172E590);
	unsigned char* randomoff = reinterpret_cast<unsigned char*>(scoreDataOffset + 2744);
	unsigned char* randomon = reinterpret_cast<unsigned char*>(scoreDataOffset + 2745);
	logger.log("hooked!");

	uint32_t score;
	uintptr_t scoreOffset = 0x172E590;
	ReadMemory(&score, scoreOffset, sizeof(score));
	
	char title[64];
	int titleOffset = 0x173EB04;
	ReadMemory(&title, titleOffset, sizeof(title));
	logger.log(std::to_string(score));
	logger.log(title);
	return fpResultsScreen(input);
}

DWORD LightfallThread() {
	
	
	logger.log("omg yippee");
	LPCSTR twoezconfig = ".\\2EZ.ini";
	int gameVer = GetPrivateProfileIntA("Settings", "GameVer", 0, twoezconfig);
	if (gameVer != 21) {
		logger.log("Game version not Final: EX, stopping\n");
		return NULL;
	}

	//Short sleep to fix crash when using legitimate data with dongle, can be overrden in ini if causing issues.
	Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoezconfig));

	if (MH_Initialize() != MH_OK) {
		logger.log("could not initialize MHook");
	}
	baseAddress = (uintptr_t)GetModuleHandleA(NULL);
	uintptr_t resultsAddr = 0x051F10;
	
	if (MH_CreateHook(reinterpret_cast<void*>(baseAddress + resultsAddr), reinterpret_cast<void*>(replacedShowResultsScreen), reinterpret_cast<void**>(&fpResultsScreen)) != MH_OK) {
		logger.log("could not create hook");
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		logger.log("could not enable hook");
	}

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

