// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include "lightfall.h"
#include "sqlite3.h"
#include <shlwapi.h>

Logger logger;
char dbPath[MAX_PATH];
uintptr_t scoreArrayAddr;

void saveScore() {
	sqlite3 *db;
	int rc;
	rc = sqlite3_open(dbPath, &db);
	if (rc) {
		logger.log(sqlite3_errmsg(db));
		return;
	}
	logger.log("c");
	char* sql =
		"INSERT INTO score(internal_title, mode, score) "
		"VALUES('asdf', 5, 83741);";
	char* errormsg;
	rc = sqlite3_exec(db, sql, NULL, NULL, &errormsg);
	if (rc != SQLITE_OK) {
		logger.log(errormsg);
		sqlite3_free(errormsg);
	}
	else {
		logger.log("Score saved");
	}
	sqlite3_close(db);
}

// stdcall to clean up stack from within function, don't have to do it in asm
void __stdcall getResultsScreenData() {
	if (readInt(battleAddr) != 0) {
		logger.log("In battle mode, skipping score");
		return;
	}
	scoredata_t scoredata = {};
	readScoreArray(scoreArrayAddr, scoredata);
	uint32_t stage = readInt(stageAddr); // 0-3 for stages 1-4, some values are offset by it
	scoredata.rate = readInt(rateAddr + (stage * 4));
	calcGrade(scoredata.grade, scoredata.rate);
	scoredata.level = readInt(levelAddr);

	char discName[128];
	readString(discName, discNameAddr + (stage * 128), 128);
	// disc name formatted either as title or title-diff if difficulty is above NM
	// string slicing to extract data
	const char dash[] = "-";
	size_t dashPos = strcspn(discName, dash);
	if (dashPos != strlen(discName)) {
		strncpy_s(scoredata.title, discName, dashPos);
		strncpy_s(scoredata.difficulty, discName + dashPos + 1, 3);
	}
	else {
		strcpy_s(scoredata.title, discName);
		strcpy_s(scoredata.difficulty, "nm");
	}

	if (6 <= scoredata.mode && scoredata.mode <= 9) { // checking if in course mode
		readString(scoredata.course_name, courseNameAddr, 128);
		strcpy_s(scoredata.difficulty, "--");
	}
}

__declspec(naked) void resScreenDetour() {
	__asm {
		mov scoreArrayAddr, edi; // edi has pointer we want
		pushad;
		pushfd; // pushing registers and flags to restore them after function
		call getResultsScreenData;
		popfd;
		popad;
		mov eax, ecx; // instructions that were replaced
		shl eax, 4; 
		jmp resScreenJumpBackAddr;
	}
}

void dbInit() {
	sqlite3 *db;
	int rc;
	rc = sqlite3_open(dbPath, &db);
	if (rc) {
		logger.log(sqlite3_errmsg(db));
		return;
	}
	char* sql =
		"CREATE TABLE IF NOT EXISTS score ("
		"id INTEGER PRIMARY KEY,"
		"time TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
		"internal_title TEXT NOT NULL,"
		"mode INTEGER NOT NULL,"
		"difficulty TEXT,"
		"level TEXT,"
		"course_name TEXT,"
		"score INTEGER NOT NULL,"
		"rate INTEGER,"
		"grade TEXT,"
		"total_notes INTEGER,"
		"kool INTEGER,"
		"cool INTEGER,"
		"good INTEGER,"
		"miss INTEGER,"
		"fail INTEGER,"
		"max_combo INTEGER);";
	char* errormsg;
	rc = sqlite3_exec(db, sql, NULL, NULL, &errormsg);
	if (rc != SQLITE_OK) {
		logger.log("Database open failed");
		logger.log(errormsg);
		sqlite3_free(errormsg);
	}
	else {
		logger.log("Database opened successfully");
	}
	sqlite3_close(db);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		LPCSTR config = ".\\lightfall.ini";
		if (GetPrivateProfileIntA("Settings", "DisableLogging", 0, config)) {
			logger.disabled = true;
		}
		GetPrivateProfileStringA("Settings", "SavePath", NULL, dbPath, MAX_PATH, config);
		if (dbPath != NULL) {
			char buff[MAX_PATH];
			strcpy_s(buff, dbPath);
			PathAppendA(buff, "lightfall.log");
			logger.open(buff);
			PathAppendA(dbPath, "scores.db");
		}
		else {
			logger.open("lightfall.log");
			strcpy_s(dbPath, "scores.db");
		}
		LPCSTR twoezconfig = ".\\2EZ.ini";
		int gameVer = GetPrivateProfileIntA("Settings", "GameVer", 0, twoezconfig);
		if (gameVer != 21) {
			logger.log("Game version not Final: EX, stopping");
			break;
		}
		// Short sleep to fix crash when using legitimate data with dongle, can be overriden in 2EZ.ini if causing issues.
		Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoezconfig));
		logger.log("hiii");
		dbInit();
		saveScore();
		// Parsing the results screen is easiest by inserting a hook in the middle of the function,
		// where a register temporarily has an important pointer to score values
		// Something like MinHook or Detours couldn't easily be used, so I manually patched in a jump instead
		uint32_t relResScreenDetourAddr = (uint32_t)((LPBYTE)resScreenDetour - (resScreenJumpAddr + 5));

		unsigned long OldProtection;
		VirtualProtect((LPVOID)(resScreenJumpAddr), 5, PAGE_EXECUTE_READWRITE, &OldProtection);
		*(BYTE*)(resScreenJumpAddr) = 0xE9; // jump opcode
		memcpy((LPVOID)(resScreenJumpAddr + 1), (LPVOID)&relResScreenDetourAddr, 4);
		VirtualProtect((LPVOID)(resScreenJumpAddr), 5, OldProtection, NULL);

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

