#pragma once
#include "Unit_CP.h"

namespace scada_ate
{
	namespace controller_module_io
	{
		class Unit_DDS_CP : public Unit_CP
		{
		protected:

			LoggerSpace::Logger* log = nullptr;
			ConfigUnitCP_DDS config;

			DomainParticipant* participant_ = nullptr;
			eprosima::fastdds::dds::Publisher* publisher_ = nullptr;
			eprosima::fastdds::dds::Subscriber* subscriber_ = nullptr;
			Topic* topic_command = nullptr;
			Topic* topic_answer = nullptr;
			DataReader* reader_command = nullptr;
			DataWriter* answerer = nullptr;

			DynamicType_ptr type_topic_command;
			DynamicType_ptr type_topic_answer;
			DynamicData_ptr data_command;
			DynamicData_ptr data_answer;

			void SetCurrentStatus(StatusUnitCP value);

			class SubListener : public DataReaderListener
			{
			public:
				Unit_DDS_CP* master;
				SubListener(Unit_DDS_CP* master) : master(master) {};
				void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override;
				void on_data_available(DataReader* reader) override;
			};
			friend class SubListener;
			std::shared_ptr<SubListener> listener_ = std::make_shared<SubListener>(this);

			ResultReqest init();
			ResultReqest clear_properties();
			ResultReqest create_type_topic_command();
			ResultReqest create_type_topic_answer();
			ResultReqest create_dynamic_data_type();
			ResultReqest init_participant();
			ResultReqest init_subscriber();
			ResultReqest init_publisher();
			ResultReqest registration_types();
			ResultReqest create_topics();
			ResultReqest Init_reader_command();
			ResultReqest Init_answerer();
			ResultReqest Clear();

			ListUsedAnswer ProcessingOfCommand(ListUsedCommand command);
			ListUsedAnswer Processing_StopTransfer_ModuleIO();
			ListUsedAnswer Processing_StartTransfer_ModuleIO();
			ListUsedAnswer Processing_Restart_ModuleIO();
			ListUsedAnswer Processing_UpdateUnits_ModuleIO();
			ListUsedAnswer Processing_Apply_UpdeteUnits_ModuleIO();
			ListUsedAnswer Processing_Terminate_Gate();
			ListUsedAnswer ResultRequestToAnswer(ResultReqest value);

			std::string CreateNameStructCommand();
			std::string CreateNameStructAnswer();
			std::string CreateNameTopicCommand(std::string source);
			std::string CreateNameTopicAnswer(std::string source);

		public:

			Unit_DDS_CP();
			~Unit_DDS_CP();
			ResultReqest InitUnitCP(std::shared_ptr<ConfigUnitCP> config, std::shared_ptr<module_io::Module_IO> unit);
			TypeUnitCP GetTypeUnitCP();
			StatusUnitCP GetCurrentStatus();

		};

	}	
}
