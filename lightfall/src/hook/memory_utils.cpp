#include <Windows.h>
#include "memory_utils.h"

namespace lightfall
{
	uint32_t readInt(uintptr_t addr)
	{
		unsigned long oldProtection;
		uint32_t res;

		VirtualProtect((LPVOID)(addr), sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &oldProtection);
		memcpy(&res, (LPVOID)(addr), sizeof(uint32_t));
		VirtualProtect((LPVOID)(addr), sizeof(uint32_t), oldProtection, NULL);

		return res;
	}

	void readChar(char* buff, uintptr_t addr, size_t size)
	{
		unsigned long oldProtection;

		VirtualProtect((LPVOID)(addr), size, PAGE_EXECUTE_READWRITE, &oldProtection);
		memcpy(buff, (LPVOID)(addr), size);
		VirtualProtect((LPVOID)(addr), size, oldProtection, NULL);
	}

	void readScoreArray(uintptr_t addr, ez2ac::scoredata_t &scoredata)
	{
		unsigned long oldProtection;
		uint32_t buff[13];

		VirtualProtect((LPVOID)(addr), sizeof(uint32_t) * 13, PAGE_EXECUTE_READWRITE, &oldProtection);
		memcpy(&buff, (LPVOID)(addr), sizeof(uint32_t) * 13);
		VirtualProtect((LPVOID)(addr), sizeof(uint32_t) * 13, oldProtection, NULL);

		scoredata.total_notes = buff[0];
		scoredata.fail = buff[2];
		scoredata.miss = buff[3];
		scoredata.good = buff[4];
		scoredata.cool = buff[5];
		scoredata.kool = buff[6];
		scoredata.max_combo = buff[8];
		scoredata.score = buff[9];

		if (scoredata.mode != 12)
		{
			strcpy_s(scoredata.grade, ez2ac::grades[buff[12]]);
		}

		else
		{
			strcpy_s(scoredata.grade, ez2ac::cv2Grades[buff[12]]);
		}
	}
}