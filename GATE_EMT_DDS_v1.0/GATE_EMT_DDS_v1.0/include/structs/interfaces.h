#pragma once
#include <structs/ServiceType.h>
#include <structs/structs.hpp>
#include <deque>

namespace atech::common
{
	class IControl
	{
	public:
		virtual uint32_t GetId() = 0;
		virtual ResultReqest GetStatus(std::deque<std::pair<uint32_t,atech::common::Status>>& st, uint32_t id = 0) = 0;
		virtual ResultReqest Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) = 0;
		virtual ResultReqest Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) = 0;
		virtual ResultReqest ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) = 0;
		virtual ~IControl() {};
	};
}

