#pragma once

#include <string>
#include "sqlite3.h"

namespace lightfall
{
	class LocalDB
	{
		public:
			~LocalDB();

			int initDB(std::string &savePath);
			void writeDB();

		private:
			sqlite3* db;
			sqlite3_stmt* stmt;
	};
}