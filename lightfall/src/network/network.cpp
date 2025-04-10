#include "network.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "../context.h"
#include "../util/logger.h"

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

		cpr::Response r = cpr::Post(cpr::Url{ "0.0.0.0" }, // todo
			cpr::Body{ msg.dump() },
			cpr::Header{ {"Content-Type", "application/json"},
				{"api-key", context.getApiKey()} }
		);
		
		if (r.status_code == 0)
		{
			log(r.error.message);
		}

		else if (r.status_code >= 400)
		{
			log("Error submitting score. (code: %ld, response: %s)", r.status_code, r.text);
		}

		else
		{
			log("Score submitted successfully.");
		}
	}
}