#pragma once

#include <windows.h>
#include <winhttp.h>

namespace lightfall
{
	class Network
	{
		public:
			void initNetwork();
			~Network();

			void submitScore();

		private:
			HINTERNET session;
			HINTERNET connection;
			HINTERNET request;
	};
}