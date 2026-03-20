#pragma once

#include <windows.h>
#include <winhttp.h>
#include "ez2ac.h"

namespace lightfall
{
	class Network
	{
		public:
			void initNetwork();
			~Network();

			void submitScore(ez2ac::scoredata_t &scoredata);

		private:
			HINTERNET session;
			HINTERNET connection;
			HINTERNET request;
	};
}