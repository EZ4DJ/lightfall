#include "score_hook.h"

#include <Windows.h>
#include "memory_utils.h"
#include "../util/logger.h"
#include "../ez2ac.h"
#include "../context.h"
#include "../network/network.h"

using lightfall::readInt;
using lightfall::readChar;

uintptr_t scoreArrayAddr;

// This should always be 0x400000 but would rather not hardcode it
uintptr_t baseAddr = (uintptr_t)GetModuleHandleA(NULL);
uintptr_t hookAddr = baseAddr + 0x542C5;
uintptr_t jumpBackAddr = hookAddr + 5;

// stdcall to clean up stack from within function, don't have to do it in asm
void __stdcall parseScore()
{
	if (readInt(ez2ac::battleAddr) != 0)
	{
		log("In battle mode, skipping score.");
		return;
	}

	ez2ac::scoredata_t scoredata = {};

	scoredata.mode = readInt(ez2ac::modeAddr);
	scoredata.stage = readInt(ez2ac::stageAddr);
	scoredata.rate = readInt(ez2ac::rateAddr + (scoredata.stage * 4));
	scoredata.level = readInt(ez2ac::levelAddr);

	lightfall::readScoreArray(scoreArrayAddr, scoredata);

	if (scoredata.mode != 12)
	{
		char discName[128];
		readChar(discName, ez2ac::discNameAddr + (scoredata.stage * 128), 128);

		// Disc name formatted either as title or title-diff if difficulty is above NM
		size_t dashPos = strcspn(discName, "-");

		if (dashPos != strlen(discName))
		{
			strncpy_s(scoredata.title, discName, dashPos);
			strncpy_s(scoredata.difficulty, discName + dashPos + 1, 3);
		}

		else
		{
			strcpy_s(scoredata.title, discName);
			strcpy_s(scoredata.difficulty, "nm");
		}
	}

	else
	{
		readChar(scoredata.title, ez2ac::cv2TitleAddr, 32);
	}

	if (6 <= scoredata.mode && scoredata.mode <= 9)
	{
		readChar(scoredata.difficulty, ez2ac::courseNameAddr, 128);
	}

	bool randomFound = false;
	for (int i = 0; i < 11; i++)
	{
		if (readInt(ez2ac::randomAddrs[i]))
		{
			randomFound = true;
			strcpy_s(scoredata.random_op, ez2ac::randomOpts[i]);
			break;
		}
	}

	if (randomFound == false)
	{
		strcpy_s(scoredata.random_op, "OFF");
	}

	if (scoredata.mode == 0 || scoredata.mode == 10 ||
		scoredata.mode == 11 || scoredata.mode == 12)
	{
		strcpy_s(scoredata.auto_op, "OFF");
	}

	else if (scoredata.mode == 5 || scoredata.mode == 9)
	{
		strcpy_s(scoredata.auto_op, ez2ac::autoOpts14K[readInt(ez2ac::autoAddr)]);
	}

	else
	{
		strcpy_s(scoredata.auto_op, ez2ac::autoOpts[readInt(ez2ac::autoAddr)]);
	}

	if (lightfall::context.getLocalMode())
	{
		lightfall::context.localDB.writeDB(scoredata);
	}

	else
	{
		lightfall::submitScore(scoredata);
	}
}

__declspec(naked) void resScreenDetour()
{
	__asm
	{
		mov scoreArrayAddr, edi; // edi has pointer we want
		pushad;
		pushfd; // Pushing registers and flags to restore them after function
		call parseScore;
		popfd;
		popad;
		mov eax, ecx; // Instructions that were overwritten
		shl eax, 4;
		jmp jumpBackAddr;
	}
}

namespace lightfall
{
	void installHook()
	{
		uint32_t relDetourAddr = (uint32_t)((LPBYTE)resScreenDetour - (hookAddr + 5));

		// Parsing score data is easiest by overwriting the middle of a function with a hook,
		// where a register temporarily has a pointer to an array of score values
		unsigned long oldProtection;

		VirtualProtect((LPVOID)(hookAddr), 5, PAGE_EXECUTE_READWRITE, &oldProtection);
		*(BYTE*)(hookAddr) = 0xE9; // jump opcode
		memcpy((LPVOID)(hookAddr + 1), (LPVOID)&relDetourAddr, 4);
		VirtualProtect((LPVOID)(hookAddr), 5, oldProtection, NULL);

		log("Hook created at address 0x%X, ready to save scores.", hookAddr);
	}
}