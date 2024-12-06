#pragma once

#include "logger.h"

Logger logger("lightfall.log");

uintptr_t baseAddress = (uintptr_t)GetModuleHandleA(NULL);

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