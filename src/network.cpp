#include "network.h"

#include <windows.h>
#include <winhttp.h>
#include "json.hpp"
#include "logger.h"
#include "context.h"

namespace lightfall
{
	void submitScore(ez2ac::scoredata_t &scoredata)
	{
		nlohmann::json msg;

		msg["title"] = scoredata.title;
		msg["mode"] = scoredata.mode;
		msg["difficulty"] = scoredata.difficulty;
		msg["level"] = scoredata.level;
		msg["score"] = scoredata.score;
		msg["rate"] = scoredata.rate;
		msg["grade"] = scoredata.grade;

		msg["total_notes"] = scoredata.total_notes;
		msg["kool"] = scoredata.kool;
		msg["cool"] = scoredata.cool;
		msg["good"] = scoredata.good;
		msg["miss"] = scoredata.miss;
		msg["fail"] = scoredata.fail;
		msg["max_combo"] = scoredata.max_combo;

		msg["random"] = scoredata.random_op;
		msg["auto"] = scoredata.auto_op;
		msg["stage"] = scoredata.stage;

		log("Submitting score for song %s...", scoredata.title);
	}
}