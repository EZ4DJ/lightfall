#pragma once

#include "../ez2ac.h"

namespace lightfall
{
	uint32_t readInt(uintptr_t addr);
	void readChar(char* buff, uintptr_t addr, size_t size);
	void readScoreArray(uintptr_t addr, ez2ac::scoredata_t &scoredata);
}