
#pragma once					
#include <Module_IO/Module_IO.hpp>
#include <Module_CTRL/UnitAid/UnitAid.hpp>
#include <ddsformat/DdsStatus/DdsStatus.h>
#include <ddsformat/DdsCommand/DdsCommand.h>
#include <ddsformat/SizeTopics.h>
#include <config/MessageSerializer.h>
#include <config/json2xml.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <Module_IO/UnitTransfer/adapters/SharedMemory_Adapter/AdapterSharedMemory.hpp>
#include <Module_IO/UnitTransfer/adapters/DDS_Adapter/AdapterDDS.hpp>
#include <Module_IO/UnitTransfer/adapters/OPCUA_Adapter/AdapterOPCUA.h>
#include <structs/ServiceType.h>
#include <Module_CTRL/UnitAid/UnitAid_DDS/UnitAid_DDS.hpp>
#include <structs/FactoryDds.h>
#include <sstream>
#include <fstream>
#include <monitor_process/atechlib/mon/include/mon/scd_process.h>
#include <config/MessageSerializer.h>;

namespace atech::srv::io::ctrl
{

	class Module_CTRL
	{
		//std::vector<std::shared_ptr<UnitAid>> vector_unitaid;
		std::shared_ptr<UnitAid> _unitaid = nullptr;
		std::shared_ptr<scada_ate::gate::Module_IO> _module_io_ptr = nullptr;
		LoggerSpaceScada::ILoggerScada_ptr log;
		int64_t _node_id = 0;
		DdsStatus function_processing(DdsCommand& command);
		std::string config_str;
		std::string config_str_new;
		std::thread thread_helper;
		std::shared_ptr<atech::srv::io::ctrl::IConfigUnitAid> _config_unitaid;
		std::shared_ptr<atech::srv::io::FactoryDDS> _factory_dds = nullptr;

		ResultReqest init_dds_layer(std::string& config);
		ResultReqest init_module_io(std::string& config);
		ResultReqest registration_size_topics(atech::common::TopicMaxSize& data);
		ResultReqest registration_dds_profiles(nlohmann::json& json);
		ResultReqest addunits_module_io(std::vector<scada_ate::gate::adapter::ConfigUnitTransfer>& vect_config_units);
		ResultReqest create_vector_config_unitstreansfer(std::vector<scada_ate::gate::adapter::ConfigUnitTransfer>& vect_config_units, std::string& stream);

		scada_ate::gate::adapter::IConfigAdapter_ptr fill_config_adapter(const atech::common::InputUnit& in);
		scada_ate::gate::adapter::IConfigAdapter_ptr fill_config_adapter(const atech::common::OutputUnit& out);
		scada_ate::gate::adapter::IConfigAdapter_ptr create_config_adapter(const std::string& type);
		ResultReqest fill_config(scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory* target, atech::common::SmConfig* source);
		ResultReqest fill_config(scada_ate::gate::adapter::dds::ConfigAdapterDDS* target, atech::common::DdsConfig* source);
		ResultReqest fill_config(scada_ate::gate::adapter::opc::ConfigAdapterOPCUA* target, atech::common::UaConfig* source);
		ResultReqest string_to_typeddsdata(scada_ate::gate::adapter::dds::TypeDDSData& type_dds, const std::string& str);
		ResultReqest string_to_security_mode_opc(scada_ate::gate::adapter::opc::SecurityMode& mode, const std::string& str);
		ResultReqest string_to_security_policy_opc(scada_ate::gate::adapter::opc::SecurityPolicy& mode, const std::string& str);
		ResultReqest vec_datum_to_vec_links(std::vector<scada_ate::gate::adapter::LinkTags>& vec_link, const std::vector<atech::common::Datum>& vec_datum);
		scada_ate::gate::adapter::TypeRegistration datum_to_linktags_typereg(const std::string& str);
		scada_ate::gate::adapter::TypeValue datum_to_linktags_typeval(const std::string str);
		ResultReqest verification_config_file(std::string& config);
		void apply_new_config(DdsCommand cmd);
		ResultReqest get_vector_id_from_json(std::vector<uint32_t>& vector, std::string_view str);
		

		DdsStatus command_apply_new_config(DdsCommand& cmd);
		DdsStatus command_receive_new_config(std::string_view parametr);
		DdsStatus command_request_version(uint32_t id);
		DdsStatus command_request_status(std::string_view parametr);
		DdsStatus command_start(std::string_view parametr);
		DdsStatus command_stop(std::string_view parametr);
		DdsStatus command_reinit(std::string_view parametr);
		DdsStatus command_request_process_info();
		DdsStatus command_set_log_level(std::string_view parametr);

	

		friend class UnitAid_DDS;

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
		ResultReqest InitUnitAid(std::shared_ptr<IConfigUnitAid> config);
												  
	};
}