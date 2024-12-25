// LIGHTFALL: an EZ2AC Final EX score tracking hook
// 

#include "lightfall.h"
#include "sqlite3.h"
#include <shlwapi.h>

Logger logger;
uintptr_t scoreArrayAddr;
uintptr_t effectorInfoAddr;
uintptr_t randomAddr;
uintptr_t autoAddr;
scoredata_t scoredata;
sqlite3* db;
sqlite3_stmt* stmt;
int inEffectorAnim = 0;

void saveScore(scoredata_t &scoredata) {
	if (sqlite3_bind_text(stmt, 1, scoredata.title, 128, 0) != SQLITE_OK) {
		logger.log("Error binding variable to SQL statement");
		logger.log(sqlite3_errmsg(db));
	}
	sqlite3_bind_int(stmt, 2, scoredata.mode);
	sqlite3_bind_text(stmt, 3, scoredata.difficulty, 4, 0);
	sqlite3_bind_int(stmt, 4, scoredata.level);
	sqlite3_bind_text(stmt, 5, scoredata.course_name, 128, 0);
	sqlite3_bind_int(stmt, 6, scoredata.score);
	sqlite3_bind_int(stmt, 7, scoredata.rate);
	sqlite3_bind_text(stmt, 8, scoredata.grade, 6, 0);
	sqlite3_bind_int(stmt, 9, scoredata.total_notes);
	sqlite3_bind_int(stmt, 10, scoredata.kool);
	sqlite3_bind_int(stmt, 11, scoredata.cool);
	sqlite3_bind_int(stmt, 12, scoredata.good);
	sqlite3_bind_int(stmt, 13, scoredata.miss);
	sqlite3_bind_int(stmt, 14, scoredata.fail);
	sqlite3_bind_int(stmt, 15, scoredata.max_combo);
	sqlite3_bind_text(stmt, 16, scoredata.random, 20, 0);
	sqlite3_bind_text(stmt, 17, scoredata.auto_op, 20, 0);
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		logger.log("Error saving score to database");
		logger.log(sqlite3_errmsg(db));
	}
	else {
		logger.log("score saved!");
	}
	if (sqlite3_reset(stmt) != SQLITE_OK) {
		logger.log("Error resetting insert statement");
		logger.log(sqlite3_errmsg(db));
	}
}

// stdcall to clean up stack from within function, don't have to do it in asm
void __stdcall getResultsScreenData() {
	if (readInt(battleAddr) != 0) {
		logger.log("In battle mode, skipping score");
		return;
	}

	scoredata = {};
	readScoreArray(scoreArrayAddr, scoredata);
	uint32_t stage = readInt(stageAddr); // 0-3 for stages 1-4, some values are offset by it
	scoredata.rate = readInt(rateAddr + (stage * 4));
	calcGrade(scoredata.grade, scoredata.rate);
	scoredata.level = readInt(levelAddr);

	char discName[128];
	readString(discName, discNameAddr + (stage * 128), 128);
	// Disc name formatted either as title or title-diff if difficulty is above NM
	// String slicing to extract data
	size_t dashPos = strcspn(discName, "-");
	if (dashPos != strlen(discName)) {
		strncpy_s(scoredata.title, discName, dashPos);
		strncpy_s(scoredata.difficulty, discName + dashPos + 1, 3);
	}
	else {
		strcpy_s(scoredata.title, discName);
		strcpy_s(scoredata.difficulty, "nm");
	}

	if ((6 <= scoredata.mode && scoredata.mode <= 9)) { // Checking if in course mode
		readString(scoredata.course_name, courseNameAddr, 128);
		strcpy_s(scoredata.difficulty, "");
	}

	strcpy_s(scoredata.random, "off");
	strcpy_s(scoredata.auto_op, "off");

	saveScore(scoredata);
}

// declspec(naked) stops function prologue/epilogue from generating
__declspec(naked) void resScreenDetour() {
	__asm {
		mov scoreArrayAddr, edi; // edi has pointer we want
		pushad;
		pushfd; // Pushing registers and flags to restore them after function
		call getResultsScreenData;
		popfd;
		popad;
		mov eax, ecx; // Instructions that were replaced
		shl eax, 4; 
		jmp resScreenJumpBackAddr;
	}
}

__declspec(naked) void animDetour() {
	__asm {
		pushfd;
		cmp inEffectorAnim, 1;
		jne skip;
		mov effectorInfoAddr, ebx; // ebx has pointer
		mov inEffectorAnim, 0;

	skip:
		popfd;
		mov edx, dword ptr ds:[esi]; // Replaced instructions
		add esi, 4;
		jmp animJumpBackAddr;
	}
}

__declspec(naked) void preEffectorsDetour() {
	__asm {
		mov inEffectorAnim, 1; // Next call of animDetour will have correct address
		push 0x495F74; // Replaced instruction
		jmp preEffectorsJumpBackAddr;
	}
}

void __stdcall getEffectors() {
	effectorInfoAddr += 0x4F0;
	uint32_t imgNamePtr = readInt(effectorInfoAddr - 4);
	char buff[128];
	readString(buff, imgNamePtr + 4, 128);
	if (strcmp(buff, "effector_random_off.bmp") == 0) {
		logger.log("Effectors found");
		uint32_t randomPtr = readInt(effectorInfoAddr + 8);
		logger.log(std::to_string(randomPtr));
		char random[30];
		readString(random, randomPtr - 31, 30);
		logger.log(random);
	}
	
}

__declspec(naked) void getEffectorsDetour() {
	__asm {
		call getEffectors;
		mov edi, 1;
		jmp getEffectorsJumpBackAddr;
	}
}

int dbInit(char dbPath[]) {
	int rc;
	rc = sqlite3_open(dbPath, &db);
	if (rc != SQLITE_OK) {
		logger.log("Error opening database");
		logger.log(sqlite3_errmsg(db));
		return 1;
	}

	char* sql =
		"CREATE TABLE IF NOT EXISTS score ("
		"id INTEGER PRIMARY KEY,"
		"time TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP," // UTC ISO-8601 timestamp
		"title TEXT NOT NULL,"
		"mode INTEGER NOT NULL,"
		"difficulty TEXT,"
		"level INTEGER,"
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
		"max_combo INTEGER,"
		"random TEXT,"
		"auto TEXT);";
	char* errormsg;
	rc = sqlite3_exec(db, sql, NULL, NULL, &errormsg);
	if (rc != SQLITE_OK) {
		logger.log("Error writing to database");
		logger.log(errormsg);
		sqlite3_free(errormsg);
		sqlite3_close(db);
		return 1;
	}
	
	sql = // Preparing insert statement
		"INSERT INTO score(title, mode, difficulty, level, course_name, score, rate, "
		"grade, total_notes, kool, cool, good, miss, fail, max_combo, random, auto) "
		"VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
	rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	if (rc != SQLITE_OK) {
		logger.log("Error preparing database insert statement");
		logger.log(sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	else {
		logger.log("Database opened successfully");
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		LPCSTR config = ".\\lightfall.ini";
		if (GetPrivateProfileIntA("Settings", "DisableLogging", 0, config)) {
			logger.disabled = true;
		}

		char dbPath[MAX_PATH];
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
		logger.log("Starting up...");

		LPCSTR twoezconfig = ".\\2EZ.ini";
		// Short sleep to fix crash when using legitimate data with dongle, can be overriden in 2EZ.ini if causing issues.
		Sleep(GetPrivateProfileIntA("Settings", "ShimDelay", 10, twoezconfig));
		if (GetPrivateProfileIntA("Settings", "GameVer", 0, twoezconfig) != 21) {
			logger.log("Game version not Final: EX, stopping");
			break;
		}
		if (dbInit(dbPath) != 0) {
			break;
		}

		// Parsing the results screen is easiest by inserting hooks in the middle of the function,
		// where registers temporarily have pointers to score values
		// Something like MinHook or Detours couldn't easily be used, so I manually patched in jumps instead
		patchJump(resScreenJumpAddr, (LPBYTE)resScreenDetour);
		patchJump(animJumpAddr, (LPBYTE)animDetour);
		patchJump(preEffectorsJumpAddr, (LPBYTE)preEffectorsDetour);
		
		logger.log("Hook created, ready to save scores");
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

