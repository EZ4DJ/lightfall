// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include "lightfall.h"
#include "MinHook.h"

#pragma comment(lib, "MinHook.x86.lib")

uint32_t* scoreArrayAddr;

// stdcall to clean up stack from within the function, don't have to do it in asm
void __stdcall getResultsScreenData() {
	logger.logByteArray(scoreArrayAddr, 15);

}

__declspec(naked) void resScreenDetour() {
	__asm {
		mov scoreArrayAddr, edi; // edi has pointer we want
		pushad;
		pushfd;
		call getResultsScreenData;
		popfd;
		popad;
		mov eax, DWORD PTR [0x1B2E548]; // instruction that was replaced
		jmp resScreenJumpBackLoc;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		logger.log("omg yippee");
		LPCSTR twoezconfig = ".\\2EZ.ini";
		int gameVer = GetPrivateProfileIntA("Settings", "GameVer", 0, twoezconfig);
		if (gameVer != 21) {
			logger.log("Game version not Final: EX, stopping");
			return NULL;
		}

		//Short sleep to fix crash when using legitimate data with dongle, can be overriden in ini if causing issues.
		Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoezconfig));

		// Parsing the results screen memory is easiest by inserting a hook in the middle of the function, where a register temporarily has an important pointer to score values
		// Something like MinHook I don't think really works here, so I manually patching in the jump instruction
		uint32_t relResScreenDetourAddr = (uint32_t)((LPBYTE)resScreenDetour - (baseAddress + resScreenJumpOffset + 5));

		unsigned long OldProtection;
		VirtualProtect((LPVOID)(baseAddress + resScreenJumpOffset), 5, PAGE_EXECUTE_READWRITE, &OldProtection);
		*(BYTE*)(baseAddress + resScreenJumpOffset) = 0xE8; // jump opcode
		memcpy((LPVOID)(baseAddress + resScreenJumpOffset + 1), (LPVOID)&relResScreenDetourAddr, 4);
		VirtualProtect((LPVOID)(baseAddress + resScreenJumpOffset + 1), 5, OldProtection, NULL);

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		MH_Uninitialize();
		break;
	}
	return TRUE;
}

