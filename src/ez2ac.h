#pragma once

#include <cstdint>

namespace ez2ac
{
	constexpr uintptr_t discNameAddr = 0x1B2EC18; // char[128]
	constexpr uintptr_t modeAddr = 0x1B2E548; // uint32, 0-12
	constexpr uintptr_t cv2TitleAddr = 0x1B2EB80; // char[32]
	constexpr uintptr_t courseNameAddr = 0x1B2E728; // char[128]
	constexpr uintptr_t levelAddr = 0x1B2EC0C; // uint32, chart number in CV2
	constexpr uintptr_t rateAddr = 0x1B2EBF8; // uint32
	constexpr uintptr_t stageAddr = 0x1B2E718; // uint32, 0-3
	constexpr uintptr_t battleAddr = 0x1B2EF6C; // uint32
	constexpr uintptr_t autoAddr = 0x1B5F178; // uint32, 0-3

	// Each random/mirror option stored in separate addresses, this was a nightmare to find
	constexpr uintptr_t randomAddrs[] = { 0x1B2E8C0, 0x1B2E8C8, 0x1B2E990, 0x1B2E8D0, 0x1B2E960, 0x1B2E998,
										  0x1B2EBBC, 0x1B2EF38, 0x1B2E958, 0x1B2E970, 0x1B2E978 };
	constexpr char* randomOpts[] = { "ON", "S", "PS", "M", "H", "H", "F", "F", "N", "K", "SP" };

	constexpr char* autoOpts[] = { "OFF", "S", "P", "SP" };
	constexpr char* autoOpts14K[] = { "OFF", "LS", "RS", "S" };

	constexpr char* grades[] = { "F", "D", "C", "B", "A", "A+", "S", "S+", "S++", "S+++", "S++++" };
	constexpr char* cv2Grades[] = { "F", "F", "D", "C", "B", "A", "A+" };

	// numbered 0-12, can use int conversion to compare to mode number stored in memory
	// the underscores are a little ugly but enums can't start with a number
	enum Mode : int 
	{ 
		_5only, 
		_5ruby, 
		_5k, 
		_7k, 
		_10k, 
		_14k, 
		_5course, 
		_7course,
		_10course, 
		_14course, 
		_catch, 
		_turntable, 
		_cv2 
	};

	struct scoredata_t
	{
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
		uint32_t stage;
	};
}