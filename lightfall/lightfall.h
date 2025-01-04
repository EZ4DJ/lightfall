#pragma once

#include "lightfall_utils.h"

uintptr_t baseAddress = (uintptr_t)GetModuleHandleA(NULL); // Should always be 0x40000 but would rather not hardcode it
constexpr uintptr_t discNameAddr = 0x1B2EC18; // char[128]
constexpr uintptr_t modeAddr = 0x1B2E548; // uint32
constexpr uintptr_t cv2SongNameAddr = 0x1B2EB80; // char[32]
constexpr uintptr_t courseNameAddr = 0x1B2E728; // char[128]
constexpr uintptr_t levelAddr = 0x1B2EC0C; // uint32, chart number in CV2
constexpr uintptr_t rateAddr = 0x1B2EBF8; // uint32
constexpr uintptr_t stageAddr = 0x1B2E718; // uint32 0-3
constexpr uintptr_t battleAddr = 0x1B2EF6C; // uint32
constexpr uintptr_t autoAddr = 0x1B5F178; // uint32 0-3

constexpr char* grades[] = { "F", "D", "C", "B", "A", "A+", "S", "S+", "S++", "S+++", "S++++" };
constexpr char* cv2Grades[] = { "F", "F", "D", "C", "B", "A", "A+" };

// Each random option stored in separate address for some reason, this was a nightmare to find
constexpr uintptr_t randomAddrs[] = { 0x1B2E8C0, 0x1B2E8C8, 0x1B2E990, 0x1B2E8D0, 0x1B2E960, 0x1B2E998,
									  0x1B2EBBC, 0x1B2EF38, 0x1B2E958, 0x1B2E970, 0x1B2E978 };
constexpr char* randomNames[] = { "ON", "S", "PS", "M", "H", "H", "F", "F", "N", "K", "SP" };
constexpr char* autoOps[] = { "OFF", "S", "P", "SP" };
constexpr char* autoOps14K[] = { "OFF", "LS", "RS", "S" };

uintptr_t resScreenJumpAddr = baseAddress + 0x0542C5;
uintptr_t resScreenJumpBackAddr = resScreenJumpAddr + 5;

struct scoredata_t {
	char title[128];
	uint32_t mode;
	char difficulty[128];
	uint32_t level;
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
	char random_op[4];
	char auto_op[4];
};

void readScoreArray(uintptr_t addr, scoredata_t &scoredata) {
	unsigned long OldProtection;
	uint32_t buff[13];
	VirtualProtect((LPVOID)(addr), sizeof(uint32_t) * 13, PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy(&buff, (LPVOID)(addr), sizeof(uint32_t) * 13);
	VirtualProtect((LPVOID)(addr), sizeof(uint32_t) * 13, OldProtection, NULL);
	scoredata.total_notes = buff[0];
	scoredata.fail = buff[2];
	scoredata.miss = buff[3];
	scoredata.good = buff[4];
	scoredata.cool = buff[5];
	scoredata.kool = buff[6];
	scoredata.max_combo = buff[8];
	scoredata.score = buff[9];
	if (scoredata.mode == 12) { // CV2 uses different grades
		strcpy_s(scoredata.grade, cv2Grades[buff[12]]);
	}
	else {
		strcpy_s(scoredata.grade, grades[buff[12]]);
	}
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