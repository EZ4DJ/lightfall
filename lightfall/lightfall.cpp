// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include <thread>
#include "lightfall.h"
#include "MinHook.h"

#pragma comment(lib, "MinHook.x86.lib")

typedef struct scoreInfo {
	char title[128];
	uint32_t mode;
	char difficulty[4];
	char course_name[128];
	uint32_t level;
	uint32_t score;
	uint32_t rate;
	char grade[5];
	uint32_t total_notes;
	uint32_t kool;
	uint32_t cool;
	uint32_t good;
	uint32_t miss;
	uint32_t fail;
	uint32_t max_combo;
};

uint32_t* scoreArrayOffset;

void __stdcall getResultsScreenData() {
	logger.logByteArray(scoreArrayOffset, 15);

}

uintptr_t resScreenJumpLoc = 0x053A85;
uintptr_t resScreenJumpBackLoc = resScreenJumpLoc + 5;
__declspec(naked) void resScreenDetour() {
	__asm {
		mov scoreArrayOffset, edi; // edi has pointer we want
		pushad;
		pushfd;
		call getResultsScreenData;
		popfd;
		popad;
		mov eax, DWORD PTR [0x1B2E548]; // the instruction that was replaced
		jmp resScreenJumpBackLoc;
	}
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

	// Parsing the results screen memory is easiest by inserting a hook in the middle of the function, where a register temporarily has an important pointer to score values
	// Tried using MinHook but couldn't get it to do what I needed, so I overwrote 5 bytes with a jump

	uint32_t relResScreenDetourAddr = (uint32_t)((LPBYTE)resScreenDetour - (baseAddress + resScreenJumpLoc + 5));

	unsigned long OldProtection;
	VirtualProtect((LPVOID)(baseAddress + resScreenJumpLoc), 5, PAGE_EXECUTE_READWRITE, &OldProtection);
	*(BYTE*)(baseAddress + resScreenJumpLoc) = 0xE8; // jump opcode
	memcpy((LPVOID)(baseAddress + resScreenJumpLoc + 1), (LPVOID)&relResScreenDetourAddr, 4);
	VirtualProtect((LPVOID)(baseAddress + resScreenJumpLoc + 1), 5, OldProtection, NULL);

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

