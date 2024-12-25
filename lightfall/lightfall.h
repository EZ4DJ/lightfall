#pragma once

#include "lightfall_utils.h"

uintptr_t baseAddress = (uintptr_t)GetModuleHandleA(NULL); // should always be 0x040000 but would rather not hardcode it
constexpr uintptr_t discNameAddr = 0x1B2EC18; // char[128]
constexpr uintptr_t modeAddr = 0x1B2E548; // uint32
constexpr uintptr_t courseNameAddr = 0x1B2E728; // char[128]
constexpr uintptr_t levelAddr = 0x1B2EC0C; // uint32
constexpr uintptr_t rateAddr = 0x1B2EBF8; // uint32
constexpr uintptr_t stageAddr = 0x1B2E718; // uint32
constexpr uintptr_t battleAddr = 0x1B2EF6C; // uint32
constexpr uintptr_t autoAddr = 0x1B5F178; // uint32

// Each random option stored in separate address for some reason, this was a nightmare to find
constexpr uintptr_t randomAddrs[] = { 0x1B2E8C0, 0x1B2E8C8, 0x1B2E990, 0x1B2E8D0, 0x1B2E960, 
									 0x1B2E998, 0x1B2EBBC, 0x1B2EF38, 0x1B2E958, 0x1B2E970, 0x1B2E978 };
constexpr char randomNames[][3] = { "on", "s", "ps", "m", "h", "h", "f", "f", "n", "k", "sp" };



uintptr_t resScreenJumpAddr = baseAddress + 0x0542C5;
uintptr_t resScreenJumpBackAddr = resScreenJumpAddr + 5;

struct scoredata_t {
	char title[128];
	uint32_t mode;
	char difficulty[4];
	uint32_t level;
	char course_name[128];
	uint32_t score;
	uint32_t rate;
	char grade[6];
	uint32_t total_notes;
	uint32_t kool;
	uint32_t cool;
	uint32_t good;
	uint32_t miss;
	uint32_t fail;
	uint32_t max_combo;
	char random[3];
	char auto_op[4];
};

void readScoreArray(uintptr_t addr, scoredata_t &scoredata) {
	unsigned long OldProtection;
	uint32_t buffer[10];
	VirtualProtect((LPVOID)(addr), sizeof(uint32_t) * 10, PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy(&buffer, (LPVOID)(addr), sizeof(uint32_t) * 10);
	VirtualProtect((LPVOID)(addr), sizeof(uint32_t) * 10, OldProtection, NULL);
	scoredata.total_notes = buffer[0];
	scoredata.fail = buffer[2];
	scoredata.miss = buffer[3];
	scoredata.good = buffer[4];
	scoredata.cool = buffer[5];
	scoredata.kool = buffer[6];
	scoredata.max_combo = buffer[8];
	scoredata.score = buffer[9];
}

// modes:
// 0: 5k only
// 1: 5k ruby
// 2: 5k standard
// 3: 7k standard
// 4: 10k maniac
// 5: 14k maniac
// 6: 5k course
// 7: 7k course
// 8: 10k course
// 9: 14k course
// 10: catch
// 11: turntable
// 12: CV2