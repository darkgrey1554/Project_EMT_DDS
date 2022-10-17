#pragma once					
#include <Module_IO/Module_IO.hpp>
#include <Module_CTRL/UnitAid/UnitAid.hpp>
#include <TypeTopicDDS/TopicStatus/TopicStatus.h>
#include <TypeTopicDDS/TopicCommand/TopicCommand.h>
#include <TypeTopicDDS/TopicSize.h>
#include <config/MessageSerializer.h>
#include <config/json2xml.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <Module_IO/UnitTransfer/adapters/SharedMemory_Adapter/AdapterSharedMemory.hpp>
#include <Module_IO/UnitTransfer/adapters/DDS_Adapter/AdapterDDS.hpp>
#include <Module_IO/UnitTransfer/adapters/OPCUA_Adapter/AdapterOPCUA.h>

#include <sstream>
#include <fstream>

namespace atech::srv::io::ctrl
{
	enum class Command
	{
		RequestStatus,
		Start,
		Stop,
		ReInit,
		ReceiveNewConfig,
		ApplyNewConfig,
		Logging
	};

	enum class Respond
	{
		Ok,
		Error,
		ErrorInit,
		ErrorReceive,
		ErrorConnect,

		Done,
		Ignor,

		Debug,
		Info,
		Warning,
		Critical
	};

	class Module_CTRL
	{
		std::vector<std::shared_ptr<UnitAid>> vector_unitaid;
		std::shared_ptr<scada_ate::gate::Module_IO> _module_io_ptr = nullptr;
		LoggerSpaceScada::ILoggerScada_ptr log;
		int64_t _node_id = 0;
		TopicStatus function_processing(TopicCommand& command);
		std::string config_str;
		std::string config_str_new;

		ResultReqest registration_size_topics(scd::common::TopicMaxSize& data);
		ResultReqest registration_dds_profiles(nlohmann::json& json);
		ResultReqest init_module_io(std::vector<scada_ate::gate::adapter::ConfigUnitTransfer>& vect_config_units);
		ResultReqest create_vector_config_unitstreansfer(std::vector<scada_ate::gate::adapter::ConfigUnitTransfer>& vect_config_units, std::string& stream);

		scada_ate::gate::adapter::IConfigAdapter_ptr fill_config_adapter(const scd::common::InputUnit& in);
		scada_ate::gate::adapter::IConfigAdapter_ptr fill_config_adapter(const scd::common::OutputUnit& out);
		scada_ate::gate::adapter::IConfigAdapter_ptr create_config_adapter(const std::string& type);
		ResultReqest fill_config(std::shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> target, std::shared_ptr<scd::common::SmConfig> source);
		ResultReqest fill_config(std::shared_ptr < scada_ate::gate::adapter::dds::ConfigAdapterDDS> target, std::shared_ptr<scd::common::DdsConfig> source);
		ResultReqest fill_config(std::shared_ptr < scada_ate::gate::adapter::opc::ConfigAdapterOPCUA> target, std::shared_ptr<scd::common::UaConfig> source);
		ResultReqest string_to_typeddsdata(scada_ate::gate::adapter::dds::TypeDDSData& type_dds, const std::string& str);
		ResultReqest string_to_security_mode_opc(scada_ate::gate::adapter::opc::SecurityMode& mode, const std::string& str);
		ResultReqest string_to_security_policy_opc(scada_ate::gate::adapter::opc::SecurityPolicy& mode, const std::string& str);
		ResultReqest vec_datum_to_vec_links(std::vector<scada_ate::gate::adapter::LinkTags>& vec_link, const std::vector<scd::common::Datum>& vec_datum);
		scada_ate::gate::adapter::TypeRegistration datum_to_linktags_typereg(const std::string& str);
		scada_ate::gate::adapter::TypeValue datum_to_linktags_typeval(const std::string str);

		Respond command_apply_new_config();
		Respond command_receive_new_config(size_t size_data);
		Respond command_request_status(size_t id);
		Respond command_start(size_t id);
		Respond command_stop(size_t id);
		Respond command_reinit(size_t id);





	public:

		Module_CTRL();
		~Module_CTRL();
		ResultReqest AddModuleIO(std::weak_ptr<scada_ate::gate::Module_IO> ptr_module_oi);
		int64_t GetNodeID();

		ResultReqest LoadConfigService();
		ResultReqest TakeConfigLogger(LoggerSpaceScada::ConfigLogger& config);
		ResultReqest InitDDSLayer();
		ResultReqest InitModuleIO();
		ResultReqest InitLogger();
		ResultReqest InitUnitAid(IConfigUnitAid* config);
												  
	};
}