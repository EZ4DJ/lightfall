#pragma once

#include <string>
#include "../ez2ac.h"
#include "sqlite3.h"

namespace lightfall
{
	class LocalDB
	{
		public:
			int initDB(std::string &savePath);
			void writeDB(ez2ac::scoredata_t &scoredata);

		private:
			sqlite3* db;
			sqlite3_stmt* stmt;
	};
}