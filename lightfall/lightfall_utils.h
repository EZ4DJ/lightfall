#pragma once

#include "logger.h"
#include <Windows.h>

uint32_t readInt(uintptr_t addr) {
	unsigned long OldProtection;
	uint32_t res;
	VirtualProtect((LPVOID)(addr), sizeof(uint32_t), PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy(&res, (LPVOID)(addr), sizeof(uint32_t));
	VirtualProtect((LPVOID)(addr), sizeof(uint32_t), OldProtection, NULL);
	return res;
}

void readString(char buffer[], uintptr_t addr, size_t size) {
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(addr), size, PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy(buffer, (LPVOID)(addr), size);
	VirtualProtect((LPVOID)(addr), size, OldProtection, NULL);
}

void calcGrade(char grade[], uint32_t rate) {
	if (rate <= 500) {
		strcpy_s(grade, 6, "F");
	}
	else if (rate <= 600) {
		strcpy_s(grade, 6, "D");
	}
	else if (rate <= 700) {
		strcpy_s(grade, 6, "C");
	}
	else if (rate <= 800) {
		strcpy_s(grade, 6, "B");
	}
	else if (rate <= 900) {
		strcpy_s(grade, 6, "A");
	}
	else if (rate <= 930) {
		strcpy_s(grade, 6, "S");
	}
	else if (rate <= 950) {
		strcpy_s(grade, 6, "S+");
	}
	else if (rate <= 980) {
		strcpy_s(grade, 6, "S++");
	}
	else if (rate <= 1000) {
		strcpy_s(grade, 6, "S+++");
	}
	else if (rate == 1000) {
		strcpy_s(grade, 6, "S++++");
	}
}