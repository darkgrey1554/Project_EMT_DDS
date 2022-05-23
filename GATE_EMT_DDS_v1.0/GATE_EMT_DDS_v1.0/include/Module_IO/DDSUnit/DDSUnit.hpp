#pragma once
#include <structs/structs.hpp>
#include "adapters/Adapters.hpp"
#include <atomic>
#include <vector>
#include "LoggerScada.hpp"
#include <thread>
#include <chrono>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>

#include <TypeTopicDDS/DDSData/DDSDataPubSubTypes.h>
#include <TypeTopicDDS/DDSDataEx/DDSDataExPubSubTypes.h>

#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastrtps/utils/IPLocator.h>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

#include <structs/TimeConverter.hpp>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::types;
using namespace std::chrono_literals;

namespace scada_ate::gate::ddsunit
{
	enum class TypeTransport
	{
		TCPv4,
		UDPv4,
		SM
	};

	enum class TypeDDSUnit
	{
		SUBSCRIBER,
		PUBLISHER
	};

	enum class CommandControlDDSUnit
	{
		NONE,
		RESTART,
		KILL,
		STOP,
		START
	};

	enum class StatusDDSUnit
	{
		EMPTY,
		ERROR_INIT,
		ERROR_DESTROYED,
		STOP,
		START,
		DESTROYED,
		WORK
	};

	enum class CommandListenerSubscriber
	{
		NONE,
		START,
		STOP
	};

	enum class TypeRecieve
	{
		LISTEN,
		REVIEW
	};

	struct ControlDDSUnit
	{
		void setCommand(CommandControlDDSUnit com)
		{
			std::lock_guard<std::mutex> guard(mut_command);
			command = com;
			return;
		};

		CommandControlDDSUnit getCommand()
		{
			std::lock_guard<std::mutex> guard(mut_command);
			return command;
		};

		void setStatus(StatusDDSUnit status)
		{
			std::lock_guard<std::mutex> guard(mut_status);
			current_status = status;
			return;
		};

		StatusDDSUnit getStatus()
		{
			std::lock_guard<std::mutex> guard(mut_status);
			return current_status;
		};

	protected:

		std::mutex mut_command;
		std::mutex mut_status;
		CommandControlDDSUnit command = CommandControlDDSUnit::NONE;
		StatusDDSUnit current_status = StatusDDSUnit::EMPTY;
	};

	/*struct InfoDDSUnit
	{
		ControlDDSUnit control;
		ConfigDDSUnit config;
	};*/

	struct ConfigDDSUnit
	{
		unsigned short Domen;
		TypeDDSUnit TypeUnit;
		TypeTransport Transport;
		std::string PointName;
		gate::adapter::TypeData Typedata;
		gate::adapter::TypeInfo Typeinfo;
		gate::adapter::TypeAdapter Adapter;
		TypeRecieve Typerecieve;
		unsigned int Frequency;
		std::string IP_MAIN;
		std::string IP_RESERVE;
		unsigned int Port_MAIN;
		unsigned int Port_RESERVE;
		std::shared_ptr<adapter::IConfigAdapter> conf_adapter;
	};

	/////////////////////////////////////////////////////////////
	///--------------- Interface DDSUnit -----------------------
	/////////////////////////////////////////////////////////////
	class IDDSUnit
	{

	protected:

		const unsigned int scatter_frequency = 25; // 25 ms 

	public:

		virtual ResultReqest Initialization() = 0;
		virtual ResultReqest Stop() = 0;
		virtual ResultReqest Start() = 0;
		virtual StatusDDSUnit GetCurrentStatus() const = 0;
		virtual ConfigDDSUnit GetConfig() const = 0;
		virtual ResultReqest SetNewConfig(ConfigDDSUnit config) = 0;
		virtual ResultReqest Restart() = 0;
		virtual ResultReqest Delete() = 0;
		virtual TypeDDSUnit GetType() const = 0;
		virtual ~IDDSUnit() {};
	};

	std::shared_ptr<IDDSUnit> CreateDDSUnit(ConfigDDSUnit config);

	/////////////////////////////////////////////////////////////
	///--------------- Interface DDSUnit_Subcriber---------------
	/////////////////////////////////////////////////////////////
	
	template<class Tkind>
	class DDSUnit_Subscriber : public IDDSUnit
	{
	private:

		ConfigDDSUnit start_config;
		ConfigDDSUnit config;
		std::shared_ptr<gate::adapter::IAdapter> AdapterUnit = nullptr;
		std::string name_unit;
		std::atomic<StatusDDSUnit> GlobalStatus = StatusDDSUnit::EMPTY;
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log;
		
		std::jthread thread_transmite;
		std::atomic<StatusThreadDSSUnit> status_thread = StatusThreadDSSUnit::NONE;
		
		DomainParticipant* participant_ = nullptr;
		eprosima::fastdds::dds::Subscriber* subscriber_ = nullptr;
		Topic* topic_data = nullptr;
		eprosima::fastdds::dds::DataReader* reader_data = nullptr;
		std::shared_ptr<Tkind> data_point;

		class SubListener : public DataReaderListener
		{
			std::atomic<CommandListenerSubscriber> status = CommandListenerSubscriber::NONE;
			long long delta_trans_last = TimeConverter::GetTime_LLmcs();
			long long delta_trans = 0;
		public:
			DDSUnit_Subscriber* master;
			SubListener(DDSUnit_Subscriber* master) : master(master) {};
			~SubListener() {};
			void Start();
			void Stop();
			void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override;
			void on_data_available(DataReader* reader) override;
		};
		friend class SubListener;
		std::shared_ptr<SubListener> listener_ = std::make_shared<SubListener>(this);
		

		void function_thread_transmite();
		void SetStatus(StatusDDSUnit status);
		std::string CreateNameTopic();
		std::string CreateNameUnit(std::string base_name);

		/// --- функция инициализации participant --- ///
		ResultReqest init_participant();

		/// --- функция инициализации subscriber --- ///
		ResultReqest init_subscriber();

		/// --- функция регистрации типа --- ///
		ResultReqest register_type();

		/// --- функция создания топика --- ///
		ResultReqest register_topic();

		/// --- функция создания reader --- ///
		ResultReqest init_reader_data();

		/// --- функция создания адапрета --- ///

		ResultReqest init_adapter();

		/// --- функция копирования днанных из массива DDS в промежуточный массив --- /// 

	public:

		DDSUnit_Subscriber(ConfigDDSUnit config);
		~DDSUnit_Subscriber();
		ResultReqest Initialization();
		ResultReqest Stop();
		ResultReqest Start();
		StatusDDSUnit GetCurrentStatus() const;
		ConfigDDSUnit GetConfig() const;
		ResultReqest SetNewConfig(ConfigDDSUnit config);
		ResultReqest Restart();
		ResultReqest Delete();
		TypeDDSUnit GetType() const;
	};

	/////////////////////////////////////////////////////////////
	///--------------- Interface DDSUnit_Publisher---------------
	/////////////////////////////////////////////////////////////
	template<class TKind>
	class DDSUnit_Publisher : public IDDSUnit
	{
		ConfigDDSUnit start_config;
		ConfigDDSUnit config;
		std::shared_ptr<adapter::IAdapter> AdapterUnit = nullptr;
		std::string name_unit;
		std::atomic<StatusDDSUnit> GlobalStatus = StatusDDSUnit::EMPTY;
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log;

		std::jthread thread_transmite;
		std::atomic<StatusThreadDSSUnit> status_thread = StatusThreadDSSUnit::NONE;

		DomainParticipant* participant_ = nullptr;
		eprosima::fastdds::dds::Publisher* publisher_ = nullptr;
		Topic* topic_data = nullptr;
		eprosima::fastdds::dds::DataWriter* writer_data = nullptr;
		std::shared_ptr<TKind> data_point;

		std::vector<long long> time_com;

		void function_thread_transmite();
		void SetStatus(StatusDDSUnit status);
		std::string CreateNameTopic();
		std::string CreateNameUnit(std::string short_name);

		/// --- функция инициализации participant --- ///
		ResultReqest init_participant();

		/// --- функция инициализации subscriber --- ///
		ResultReqest init_publisher();

		/// --- функция регистрации типа --- ///
		ResultReqest register_type();

		/// --- функция создания топика --- ///
		ResultReqest register_topic();

		/// --- функция создания reader --- ///
		ResultReqest init_writer_data();

		/// --- функция создания топика --- ///
		ResultReqest init_adapter();

	public:

		DDSUnit_Publisher(ConfigDDSUnit config);
		~DDSUnit_Publisher();

		ResultReqest Initialization();
		ResultReqest Stop();
		ResultReqest Start();
		StatusDDSUnit GetCurrentStatus() const;
		ConfigDDSUnit GetConfig() const;
		ResultReqest SetNewConfig(ConfigDDSUnit config);
		ResultReqest Restart();
		ResultReqest Delete();
		TypeDDSUnit GetType() const;
	};
}



