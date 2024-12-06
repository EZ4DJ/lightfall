// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include <thread>
#include "lightfall.h"
#include "MinHook.h"

#pragma comment(lib, "MinHook.x86.lib")

uintptr_t baseAddress;
uintptr_t lightfallAddress;

void insertAsm(uintptr_t baseAddress, uintptr_t offset, uint8_t assembly[], size_t size) {
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(baseAddress + offset), size, PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy((LPVOID)(baseAddress + offset), assembly, size);
	VirtualProtect((LPVOID)(baseAddress + offset), size, OldProtection, NULL);
}



void getResultsScreenData() {
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
	
	//uintptr_t resultsAddr = 0x051F10;

	uintptr_t callLoc = 0x05438B;
	uint8_t test[5] = {0x39, 0x39, 0x39, 0x39, 0x39};
	lightfallAddress = (uintptr_t)GetModuleHandleA("lightfall.dll");
	logger.log(std::to_string(lightfallAddress));
	uintptr_t getResultsScreenDataAddr = lightfallAddress + reinterpret_cast<uintptr_t>(getResultsScreenData);
	uint8_t getResultsScreenData[5];
	insertAsm(baseAddress, callLoc, getResultsScreenDataAddr, 5);

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

