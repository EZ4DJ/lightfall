#include <Windows.h>
#include <shlwapi.h>
#include "local_db.h"
#include "../util/logger.h"

namespace lightfall
{
	int LocalDB::initDB(std::string savePath)
	{
		int rc;

		char dbPath[MAX_PATH];
		strcpy_s(dbPath, savePath.c_str());
		PathAppendA(dbPath, "scores.db");

		rc = sqlite3_open(dbPath, &db);

		if (rc != SQLITE_OK)
		{
			log("Error opening database: " + std::string(sqlite3_errmsg(db)));
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
			"auto TEXT,"
			"stage INTEGER);";

		char* errormsg;

		rc = sqlite3_exec(db, sql, NULL, NULL, &errormsg);

		if (rc != SQLITE_OK)
		{
			log("Error writing to database: " + std::string(errormsg));
			sqlite3_free(errormsg);
			sqlite3_close(db);

			return 1;
		}

		sql = "INSERT INTO score(title, mode, difficulty, level, score, rate, grade, "
			  "total_notes, kool, cool, good, miss, fail, max_combo, random, auto, stage) "
			  "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

		rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

		if (rc != SQLITE_OK)
		{
			log("Error preparing database insert statement: " + std::string(sqlite3_errmsg(db)));
			sqlite3_close(db);

			return 1;
		}

		else
		{
			log("Database opened successfully.");
		}

		return 0;
	}

	void LocalDB::writeDB(ez2ac::scoredata_t &scoredata)
	{
		if (sqlite3_bind_text(stmt, 1, scoredata.title, -1, 0) != SQLITE_OK)
		{
			log("Error binding variable to insert statement: " + std::string(sqlite3_errmsg(db)));
		}

		sqlite3_bind_int(stmt, 2, scoredata.mode);
		sqlite3_bind_text(stmt, 3, scoredata.difficulty, -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 4, scoredata.level);
		sqlite3_bind_int(stmt, 5, scoredata.score);
		sqlite3_bind_int(stmt, 6, scoredata.rate);
		sqlite3_bind_text(stmt, 7, scoredata.grade, -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 8, scoredata.total_notes);
		sqlite3_bind_int(stmt, 9, scoredata.kool);
		sqlite3_bind_int(stmt, 10, scoredata.cool);
		sqlite3_bind_int(stmt, 11, scoredata.good);
		sqlite3_bind_int(stmt, 12, scoredata.miss);
		sqlite3_bind_int(stmt, 13, scoredata.fail);
		sqlite3_bind_int(stmt, 14, scoredata.max_combo);
		sqlite3_bind_text(stmt, 15, scoredata.random_op, -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 16, scoredata.auto_op, -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 17, scoredata.stage);

		if (sqlite3_step(stmt) != SQLITE_DONE)
		{
			log("Error saving score to database: " + std::string(sqlite3_errmsg(db)));
		}

		else
		{
			log("Score saved for song: " + std::string(scoredata.title) + "\n");
		}

		if (sqlite3_reset(stmt) != SQLITE_OK)
		{
			log("Error resetting insert statement: " + std::string(sqlite3_errmsg(db)));
		}
	}
}