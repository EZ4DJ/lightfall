#pragma once

#include <cstdint>
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

void patchHook(uintptr_t targetAddr, LPBYTE detourAddr) {
	// Calculating relative address to detour function
	uint32_t relDetourAddr = (uint32_t)(detourAddr - (targetAddr + 5));

	unsigned long OldProtection;
	VirtualProtect((LPVOID)(targetAddr), 5, PAGE_EXECUTE_READWRITE, &OldProtection);
	*(BYTE*)(targetAddr) = 0xE9; // jump opcode
	memcpy((LPVOID)(targetAddr + 1), (LPVOID)&relDetourAddr, 4);
	VirtualProtect((LPVOID)(targetAddr), 5, OldProtection, NULL);
}