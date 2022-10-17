#pragma once
#include <structs/structs.hpp>
#include <TypeTopicDDS/TopicCommand/TopicCommand.h>
#include <TypeTopicDDS/TopicStatus/TopicStatus.h>
#include <queue>
#include <LoggerScada.hpp>


namespace atech::srv::io::ctrl
{

	enum class TypeUnitAid
	{
		DDS,
		TCP,
		Nope
	};

	enum class StatusUnitAid
	{
		OK,
		INITIALIZATION,
		ERROR_INIT,
		Nope
	};

	struct IConfigUnitAid
	{
		TypeUnitAid type_aid = TypeUnitAid::Nope;
		uint32_t node_id = 0;
		TopicStatus (*func)(TopicCommand& command) = nullptr;
		virtual ~IConfigUnitAid() {};
	};

	class UnitAid
	{

	protected:

		std::atomic<StatusUnitAid> _status = StatusUnitAid::Nope;
		std::mutex _mutex_init;

	public:

		virtual ~UnitAid() {};
		virtual StatusUnitAid GetStatus() = 0;
		virtual ResultReqest RespondStatus(TopicStatus& status) = 0;
		virtual ResultReqest InitUnitAid() = 0;
	};

	std::shared_ptr<UnitAid> CreateUnit_CP(std::shared_ptr<IConfigUnitAid> config);
	
}