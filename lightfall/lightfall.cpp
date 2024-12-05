// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include <thread>
#include "lightfall.h"
#include "MinHook.h"

#pragma comment(lib, "MinHook.x86.lib")

uintptr_t baseAddress;

void(*resScreenFunc)() = NULL;
void resScreenFuncIntercept() {
	uintptr_t scoreArrayOffset;
	__asm {
		mov scoreArrayOffset, edi;
		pushad;
	}
	logger.log(std::to_string(scoreArrayOffset));
	logger.log("hooked!");

	uintptr_t scoreDataOffset = baseAddress + 0x95C5C;
	uint32_t* scorep = reinterpret_cast<uint32_t*>(baseAddress + 0x172E590);
	
	uint32_t score;
	uintptr_t scoreOffset = 0x172E590;
	readMemory(&score, baseAddress, scoreOffset, sizeof(score));
	
	logger.log(std::to_string(score));

	uintptr_t resScreenReturnAddr = 0x050690;
	resScreenReturnAddr += baseAddress;
	__asm popad; 
	resScreenFunc();
}

DWORD LightfallThread() {
	logger.log("omg yippee");
	LPCSTR twoezconfig = ".\\2EZ.ini";
	int gameVer = GetPrivateProfileIntA("Settings", "GameVer", 0, twoezconfig);
	if (gameVer != 21) {
		logger.log("Game version not Final: EX, stopping");
		return NULL;
	}

	//Short sleep to fix crash when using legitimate data with dongle, can be overriden in ini if causing issues.
	Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoezconfig));

	if (MH_Initialize() != MH_OK) {
		logger.log("Could not initialize MHook");
	}
	baseAddress = (uintptr_t)GetModuleHandleA(NULL);
	logger.log(std::to_string(baseAddress));
	uintptr_t resScreenAddr = 0x051F10;
	
	if (MH_CreateHook((LPVOID)(baseAddress + resScreenAddr), (LPVOID)(resScreenFuncIntercept), (void**)(&resScreenFunc)) != MH_OK) {
		logger.log("Could not create hook");
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		logger.log("Could not enable hook");
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
		MH_Uninitialize();
		break;
	}
	return TRUE;
}

