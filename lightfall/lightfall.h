#pragma once

#include "logger.h"

Logger logger("lightfall.log");

uintptr_t baseAddress = (uintptr_t)GetModuleHandleA(NULL);
constexpr uintptr_t discNameAddr = 0x1B2EC18;
constexpr uintptr_t modeAddr = 0x1B2E548;
constexpr uintptr_t courseNameAddr = 0x1B2E728;
constexpr uintptr_t levelAddr = 0x1B2EC0C;
constexpr uintptr_t inCV2 = 0x1B2EB6C;

constexpr uintptr_t resScreenJumpOffset = 0x053A85;
uintptr_t resScreenJumpBackAddr = baseAddress + resScreenJumpOffset + 5;

struct scoreInfo {
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

void readString(uintptr_t offset, char buffer[], size_t size) {
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(baseAddress + offset), size, PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy(buffer, (LPVOID)(baseAddress + offset), size);
	VirtualProtect((LPVOID)(baseAddress + offset), size, OldProtection, NULL);
}

uint32_t readInt(uintptr_t offset) {
	unsigned long OldProtection;
	uint32_t res;
	VirtualProtect((LPVOID)(baseAddress + offset), sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy(&res, (LPVOID)(baseAddress + offset), sizeof(uint32_t));
	VirtualProtect((LPVOID)(baseAddress + offset), sizeof(uint32_t), OldProtection, NULL);
	return res;
}