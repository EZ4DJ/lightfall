#pragma once

#include "logger.h"

Logger logger("lightfall.log");

void readMemory(void* dest, uintptr_t baseAddress, uintptr_t offset, size_t size) {
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(baseAddress + offset), size, PAGE_EXECUTE_READWRITE, &OldProtection);
	logger.log(std::to_string(OldProtection));
	CopyMemory(dest, (LPVOID)(baseAddress + offset), size);
	VirtualProtect((LPVOID)(baseAddress + offset), size, OldProtection, NULL);
}