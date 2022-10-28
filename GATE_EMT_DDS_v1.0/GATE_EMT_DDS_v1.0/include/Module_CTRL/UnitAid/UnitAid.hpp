#pragma once
#include <structs/structs.hpp>
#include <ddsformat/DdsCommand/DdsCommand.h>
#include <ddsformat/DdsStatus/DdsStatus.h>
#include <queue>
#include <LoggerScada.hpp>



namespace atech::srv::io::ctrl
{
	class Module_CTRL;

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
		Module_CTRL* manager;
		TypeUnitAid type_aid = TypeUnitAid::Nope;
		uint32_t node_id = 0;
		virtual ~IConfigUnitAid() {};
	};

	class UnitAid
	{

	protected:

		std::atomic<StatusUnitAid> _status = StatusUnitAid::Nope;
		std::mutex _mutex_init;
		virtual DdsStatus broadcast_command(DdsCommand& cmd) = 0;

	public:

		virtual ~UnitAid() {};
		virtual StatusUnitAid GetStatus() = 0;
		virtual ResultReqest TakeServiceConfig(size_t size_data, std::string& str) = 0;
		virtual ResultReqest RespondStatus(DdsStatus& status) = 0;
		virtual ResultReqest InitUnitAid() = 0;
	};

	std::shared_ptr<UnitAid> CreateUnit_CP(std::shared_ptr<IConfigUnitAid> config);
	
}