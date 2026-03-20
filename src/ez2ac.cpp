#include "ez2ac.h"

namespace ez2ac
{
	const char* randomOpts[] = {
		"ON", // these are ordered by addresses in randomAddrs
		"S",
		"PS",
		"M",
		"H",
		"H",
		"F",
		"F",
		"N",
		"K",
		"SP"
	};

	const char* autoOpts[] = {
		"OFF",
		"S",
		"P",
		"SP"
	};

	const char* autoOpts14K[] = {
		"OFF",
		"LS",
		"RS",
		"S"
	};

	const char* grades[] = {
		"F",
		"D",
		"C",
		"B",
		"A",
		"A+",
		"S",
		"S+",
		"S++",
		"S+++",
		"S++++"
	};

	const char* cv2Grades[] = {
		"F",
		"F",
		"D",
		"C",
		"B",
		"A",
		"A+"
	};

	scoredata_t scoredata{};
}