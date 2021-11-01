#include "ModuleIO.h"

namespace scada_ate
{

	Module_IO::Module_IO()
	{
		log = LoggerSpace::Logger::getpointcontact();
	}

	Module_IO::~Module_IO()
	{

	}



	ResultReqest Module_IO::Clear()
	{
		std::string helpstr;

		try
		{
			/// --- удаление DDSunits ---///

			Map_DDSUnits.clear();

			/// --- очистка топиков публикаций и подписок --- ///

			if (!answerer)
			{
				if (publisher_->delete_datawriter(answerer) != ReturnCode_t::RETCODE_OK) throw 1;
				answerer = nullptr;
			}

			if (!reader_command)
			{
				if (subscriber_->delete_datareader(reader_command) != ReturnCode_t::RETCODE_OK) throw 2;
				reader_command = nullptr;
			}

			if (!publisher_)
			{
				if (participant_->delete_publisher(publisher_) != ReturnCode_t::RETCODE_OK)  throw 3;
				publisher_ = nullptr;
			}

			if (!subscriber_)
			{
				if (participant_->delete_subscriber(subscriber_) != ReturnCode_t::RETCODE_OK) throw 4;
				subscriber_ = nullptr;
			}

			if (!topic_command)
			{
				if (participant_->delete_topic(topic_command) != ReturnCode_t::RETCODE_OK) throw 5;
				topic_command = nullptr;
			}

			if (!topic_answer)
			{
				if (participant_->delete_topic(topic_answer) != ReturnCode_t::RETCODE_OK) throw 6;
				topic_answer = nullptr;
			}

			if (!topic_InfoDDSUnit)
			{
				if (participant_->delete_topic(topic_InfoDDSUnit) != ReturnCode_t::RETCODE_OK) throw 6;
				topic_InfoDDSUnit = nullptr;
			}

			/// --- удаление participant --- ///  

			if (!participant_)
			{
				participant_->unregister_type(CreateNameTopicCommand(config_gate.TopicSubscribtionInfoConfig));
				participant_->unregister_type(CreateNameTopicAnswer(config_gate.TopicPublicationAnswer));
				participant_->unregister_type(CreateNameTopicConfigDDSUnits());
			}

			if (!participant_)
			{
				if (DomainParticipantFactory::get_instance()->delete_participant(participant_) != ReturnCode_t::RETCODE_OK) throw 7;
				participant_ = nullptr;
			}

			
		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr = "Error ModuleIO : Error command Clear";
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error ModuleIO : Error command Clear";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		SetCurrentStatus(StatusModeluIO::Null);
		return ResultReqest::OK;
	};

	ResultReqest Module_IO::StopTransfer()
	{
		StatusModeluIO stat;
		ResultReqest res = ResultReqest::OK;
		int count_ = 0;
		std::string helpstr;

		try
		{
			stat = GetCurrentStatus();
			if (stat != StatusModeluIO::WORK) throw 1;

			for (auto i = Map_DDSUnits.begin(); i != Map_DDSUnits.end();)
			{
				if (i->second->Stop() != ResultReqest::OK) count_++;
				i++;
			}
		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr = "Error ModuleIO : Error command Stop_transfer";
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error ModuleIO : Error command Stop_transfer";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		if (count_ != 0)
		{
			helpstr.clear();
			helpstr = "Warning ModuleIO : Error command Stop_Transfer: not all units is stoped";
			log->WriteLogWARNING(helpstr.c_str(), count_, 0);
		}

		SetCurrentStatus(StatusModeluIO::STOP);
		return res;
	}

	ResultReqest Module_IO::StartTransfer()
	{
		StatusModeluIO stat;
		ResultReqest res = ResultReqest::OK;
		int count_ = 0;
		std::string helpstr;

		try
		{
			stat = GetCurrentStatus();
			if (stat != StatusModeluIO::STOP) throw 1;

			for (auto i = Map_DDSUnits.begin(); i != Map_DDSUnits.end();)
			{
				if (i->second->Start() != ResultReqest::OK) count_++;
				i++;
			}
		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr = "Error ModuleIO : Error command StartTransfer";
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error ModuleIO : Error command StartTransfer";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		if (count_ != 0)
		{
			helpstr.clear();
			helpstr = "Warning ModuleIO : Error command StartTransfer: not all units is started";
			log->WriteLogWARNING(helpstr.c_str(), count_, 0);
		}

		SetCurrentStatus(StatusModeluIO::WORK);
		return res;
	}

	ResultReqest Module_IO::ReInitModule()
	{
		std::string helpstr;
		if (Clear() != ResultReqest::OK)
		{
			return ResultReqest::ERR;
		}

		if (init() != ResultReqest::OK)
		{
			return ResultReqest::ERR;
		}
		
		return ResultReqest::OK;
	}

	ResultReqest Module_IO::InitModule()
	{
		return init();
	}


	ResultReqest Module_IO::UpdateUnits()
	{

	}


	ResultReqest Module_IO::init()
	{
		std::string helpstr;
		ResultReqest res;
		StatusModeluIO status_ = GetCurrentStatus();

		if (status_ == StatusModeluIO::WORK || status_ == StatusModeluIO::STOP)
		{
			helpstr.clear();
			helpstr += "Error ModuleIO:Initialization already done";
			log->WriteLogWARNING(helpstr.c_str(), 0, 0);
			return ResultReqest::IGNOR;
		}

		res = clear_properties();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- чтение конфигурации --- /// 
		res = reader_config->ReadConfigGATE(config_gate);
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- инициализация типа топика команд --- ///

		res = create_type_topic_command();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- инициализация типа топика ответа --- ///

		res = create_type_topic_answer();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- инициализация типа топика конфигурации dds units --- ///

		res = create_type_topic_infoddsunits();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- инициализация participant --- ///

		res = init_participant();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- создание subscriber --- ///

		res = init_subscriber();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- создание publisher --- ///

		res = init_publisher();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- регистрация типов --- ///

		res = registration_types();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- регистрация topics--- ///

		res = create_topics();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- обновление конфигурации DDSUnits ---- /// 
		res = UpdateConfigDDSUnits();
		if (res != ResultReqest::OK)
		{
			/// ---- ///
		}

		/// --- чтение конфигурации --- ///

		res = reader_config->ReadConfigTransferUnits(config_DDSUnits);
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- инициадизация reader_command --- ///

		res = Init_reader_command();
		if (res != ResultReqest::OK)
		{
			SetCurrentStatus(StatusModeluIO::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- init_ddsunits --- ///

		for (std::vector<ConfigDDSUnit>::iterator i = config_DDSUnits.begin(); i != config_DDSUnits.end();)
		{
			helpstr.clear();
			if (i->TypeUnit == TypeDDSUnit::PUBLISHER) { helpstr += "Publisher"; }
			else if (i->TypeUnit == TypeDDSUnit::PUBLISHER) { helpstr += "Subscriber"; };
			helpstr += ':' + i->PointName;

			Map_DDSUnits.insert({ helpstr, gate::CreateDDSUnit(*i) });
			i++;
		}

		for (auto i = Map_DDSUnits.begin(); i != Map_DDSUnits.end();)
		{
			i->second->Initialization();
			i++;
		}

		SetCurrentStatus(StatusModeluIO::WORK);
		return ResultReqest::OK;
	};

	ResultReqest  Module_IO::clear_properties()
	{
		ReturnCode_t res;
		std::string helpstr;

		try
		{
			if (!reader_command)
			{
				res = subscriber_->delete_datareader(reader_command);
				if (res != ReturnCode_t::RETCODE_OK); throw 1;
				reader_command = nullptr;
			}

			if (!answerer)
			{
				res = publisher_->delete_datawriter(answerer);
				if (res != ReturnCode_t::RETCODE_OK); throw 2;
				answerer = nullptr;
			}

			if (!subscriber_)
			{
				res = participant_->delete_subscriber(subscriber_);
				if (res != ReturnCode_t::RETCODE_OK); throw 3;
				subscriber_ = nullptr;
			}

			if (!publisher_)
			{
				res = participant_->delete_publisher(publisher_);
				if (res != ReturnCode_t::RETCODE_OK); throw 4;
				publisher_ = nullptr;
			}

			if (!topic_command)
			{
				res = participant_->delete_topic(topic_command);
				if (res != ReturnCode_t::RETCODE_OK); throw 5;
				topic_command = nullptr;
			}

			if (!topic_answer)
			{
				res = participant_->delete_topic(topic_answer);
				if (res != ReturnCode_t::RETCODE_OK); throw 6;
				topic_answer = nullptr;
			}

			if (!topic_InfoDDSUnit)
			{
				res = participant_->delete_topic(topic_InfoDDSUnit);
				if (res != ReturnCode_t::RETCODE_OK); throw 6;
				topic_InfoDDSUnit = nullptr;
			}

			if (participant_)
			{
				res = DomainParticipantFactory::get_instance()->delete_participant(participant_);
				if (res != ReturnCode_t::RETCODE_OK); throw 7;
				participant_ = nullptr;
			}
		}
		catch (int& e_int)
		{
			helpstr.clear();
			helpstr = "Error init ModuleIO : Error clear proporties";
			log->WriteLogERR(helpstr.c_str(), e_int, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error init ModuleIO : Error clear proporties";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest  Module_IO::create_type_topic_command()
	{
		/* struct:
		*	id_gate : uint32;
			number_command : uint32;
			parametr : uint32;
		*/
		std::string helpstr;

		DynamicTypeBuilder_ptr created_type_id_gate;
		DynamicTypeBuilder_ptr created_type_command;
		DynamicTypeBuilder_ptr created_type_parametr;
		DynamicTypeBuilder_ptr struct_type_builder;

		try
		{
			try
			{
				created_type_id_gate = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				created_type_command = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				created_type_parametr = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				struct_type_builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
			}
			catch (...)
			{
				throw 1;
			}

			try
			{
				struct_type_builder->add_member(0, "id_gate", created_type_id_gate.get());
				struct_type_builder->add_member(1, "number_command", created_type_command.get());
				struct_type_builder->add_member(2, "parametr", created_type_parametr.get());
				helpstr.clear();
				helpstr += CreateNameStructCommand();
				struct_type_builder->set_name(helpstr);
			}
			catch (...)
			{
				throw 2;
			}

			try
			{
				type_topic_command = struct_type_builder->build();
			}
			catch (...)
			{
				throw 3;
			}
		}
		catch (int& e_int)
		{
			helpstr.clear();
			helpstr = "Error init ModuleIO : Error create type topic command";
			log->WriteLogERR(helpstr.c_str(), e_int, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error init ModuleIO : Error create type topic command";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	}

	ResultReqest  Module_IO::create_type_topic_answer()
	{
		/* struct:
		*	id_gate : uint32;
			number_command : uint32;
			result : char;
			parametr : uint32;
		*/
		std::string helpstr;

		DynamicTypeBuilder_ptr created_type_id_gate;
		DynamicTypeBuilder_ptr created_type_command;
		DynamicTypeBuilder_ptr created_type_result;
		DynamicTypeBuilder_ptr created_type_parametr;
		DynamicTypeBuilder_ptr struct_type_builder;

		try
		{
			try
			{
				created_type_id_gate = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				created_type_command = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				created_type_result = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
				created_type_parametr = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				struct_type_builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
			}
			catch (...)
			{
				throw 1;
			}

			try
			{
				struct_type_builder->add_member(0, "id_gate", created_type_id_gate.get());
				struct_type_builder->add_member(1, "number_command", created_type_command.get());
				struct_type_builder->add_member(2, "result", created_type_result.get());
				struct_type_builder->add_member(3, "parametr", created_type_parametr.get());
				helpstr.clear();
				helpstr += CreateNameStructAnswer();
				struct_type_builder->set_name(helpstr);
			}
			catch (...)
			{
				throw 2;
			}

			try
			{
				type_topic_answer = struct_type_builder->build();
			}
			catch (...)
			{
				throw 3;
			}
		}
		catch (const int& e_int)
		{
			helpstr.clear();
			helpstr = "Error init ModuleIO : Error create type topic answer";
			log->WriteLogERR(helpstr.c_str(), e_int, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error init ModuleIO : Error create type topic answer";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest  Module_IO::create_type_topic_infoddsunits()
	{
		/* struct:
		*	id_gate : uint32;
			size_inform: unint32 (size of byte)
		*/
		std::string helpstr;

		DynamicTypeBuilder_ptr created_type_id_gate;
		DynamicTypeBuilder_ptr created_type_sizeinform;
		DynamicTypeBuilder_ptr struct_type_builder;

		try
		{
			try
			{
				created_type_id_gate = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				created_type_sizeinform = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				struct_type_builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
			}
			catch (...)
			{
				throw 1;
			}

			try
			{
				struct_type_builder->add_member(0, "id_gate", created_type_id_gate.get());
				struct_type_builder->add_member(1, "number_command", created_type_sizeinform.get());
				helpstr.clear();
				helpstr += CreateNameStructInfoUnits();
				struct_type_builder->set_name(helpstr);
			}
			catch (...)
			{
				throw 2;
			}

			try
			{
				type_topic_infoddsunits = struct_type_builder->build();
			}
			catch (...)
			{
				throw 3;
			}
		}
		catch (const int& e_int)
		{
			helpstr.clear();
			helpstr = "Error init ModuleIO : Error create type topic answer";
			log->WriteLogERR(helpstr.c_str(), e_int, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error init ModuleIO : Error create type topic answer";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest Module_IO::init_participant()
	{
		std::string helpstr;

		/// --- инициализация транспортного уровня --- ///
		///--------------------------------------------///
		//////////////////////////////////////////////////

		/// --- иницализация participant --- /// 	

		try
		{
			participant_ =
				DomainParticipantFactory::get_instance()->create_participant(config_gate.Domen, PARTICIPANT_QOS_DEFAULT, nullptr);
			if (!participant_) throw 2;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error ModuleIO: Error create of participant";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest Module_IO::init_subscriber()
	{
		std::string helpstr;

		try
		{
			subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
			if (!subscriber_) throw - 1;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error Module_IO: Error create of subscriber";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest Module_IO::init_publisher()
	{
		std::string helpstr;

		try
		{
			publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
			if (!publisher_) throw - 1;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error Module_IO: Error create of publisher";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest Module_IO::registration_types()
	{
		try
		{
			TypeSupport PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(type_topic_command);
			PtrSupporType.get()->auto_fill_type_information(false);
			PtrSupporType.get()->auto_fill_type_object(true);
			if (PtrSupporType.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 1;

			PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(type_topic_answer);
			PtrSupporType.get()->auto_fill_type_information(false);
			PtrSupporType.get()->auto_fill_type_object(true);
			if (PtrSupporType.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 2;

			PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(type_topic_infoddsunits);
			PtrSupporType.get()->auto_fill_type_information(false);
			PtrSupporType.get()->auto_fill_type_object(true);
			if (PtrSupporType.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 3;

		}
		catch (int& e)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error ModeleIO: Error registration of type";
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error registration of type";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest Module_IO::create_topics()
	{
		std::string helpstr;
		try
		{
			topic_command = participant_->
				create_topic(CreateNameTopicCommand(config_gate.TopicSubscritionCommand), CreateNameStructCommand(), TOPIC_QOS_DEFAULT);
			if (!topic_command) throw 1;

			topic_answer = participant_->
				create_topic(CreateNameTopicAnswer(config_gate.TopicPublicationAnswer), CreateNameStructAnswer(), TOPIC_QOS_DEFAULT);
			if (!topic_answer) throw 2;

			topic_InfoDDSUnit = participant_->
				create_topic(CreateNameTopicInfoUnits(config_gate.TopicSubscribtionInfoConfig), CreateNameStructInfoUnits(), TOPIC_QOS_DEFAULT);
			if (!topic_InfoDDSUnit) throw 3;

		}
		catch (...)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error init ModuleIO: Error create topics";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest Module_IO::UpdateConfigDDSUnits()
	{
		DataReader* reader_config = nullptr;
		DynamicData_ptr infounits;
		DynamicData_ptr array_config_file;
		DynamicType_ptr type_array_config_file;
		DynamicTypeBuilder_ptr builder_array;
		Topic* topic_config_file=nullptr;
		SampleInfo info_request;
		unsigned int size_data;
		unsigned int id;
		char* mass_ptr = nullptr;
		MemberId pos0_array;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (!participant_) throw 1;
			if (!subscriber_) throw 2;

			/// --- создание переменной для считывания информации из топика infoddsunits --- ///
			infounits = DynamicDataFactory::get_instance()->create_data(type_topic_infoddsunits);
			/// --- создание reader для считывания данных из топика  infoddsunits--- ///
			reader_config = subscriber_->create_datareader(topic_InfoDDSUnit, DATAREADER_QOS_DEFAULT, nullptr);
			if (reader_config == nullptr) throw 3;

			/// --- чтение данных --- ///
			if (reader_config->take_next_sample(infounits.get(), &info_request) == ReturnCode_t::RETCODE_OK) throw 4;

			/// --- проверка данных --- ///
			id = infounits->get_uint32_value(0);
			if (id != config_gate.IdGate) throw 5;
			size_data = infounits->get_uint32_value(1);
			if (size_data == 0) throw 6;

			/// --- удаление reader --- ///
			subscriber_->delete_datareader(reader_config);
			reader_config = nullptr;

			/// --- массив для хранения информации по ddsunits --- ///
			std::shared_ptr<char> mass_data(new char[(uint64_t)size_data + 1], std::default_delete<char[]>());

			/// --- создвние типа данных и переменной для считывания топика с информацией по ddsunits ---///
			std::vector<uint32_t>lengths = { 1, size_data };
			builder_array = DynamicTypeBuilderFactory::get_instance()->create_array_builder(
				DynamicTypeBuilderFactory::get_instance()->create_char8_type(), lengths);
			builder_array->set_name("type_FileConfigDDSUnits");
			type_array_config_file = builder_array->build();
			array_config_file = DynamicDataFactory::get_instance()->create_data(type_array_config_file);

			/// --- регистриция типа данных --- ///
			TypeSupport PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(type_array_config_file);
			PtrSupporType.get()->auto_fill_type_information(false);
			PtrSupporType.get()->auto_fill_type_object(true);
			if (PtrSupporType.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 7;

			/// --- регистрация топика ---- /// 
			topic_config_file = participant_->create_topic(CreateNameTopicConfigDDSUnits(), type_array_config_file->get_name(), TOPIC_QOS_DEFAULT);
			if (!topic_config_file) throw 8;

			/// --- создание reader topic_config_file ---- /// 
			reader_config = subscriber_->create_datareader(topic_config_file, DATAREADER_QOS_DEFAULT, nullptr);
			if (!reader_config) throw 9;

			/// --- чтения данных из топика конфигурации
			if (reader_config->take_next_sample(array_config_file.get(), &info_request) == ReturnCode_t::RETCODE_OK) throw 10;

			/// --- копирование данных в буфер mass_ptr --- //// 
			mass_ptr = mass_data.get();
			pos0_array = array_config_file->get_array_index({ 0, 0 });
			for (int i = 0; i < size_data; i++)
			{
				*(mass_ptr + i) = array_config_file->get_char8_value(pos0_array + i);
			}
			*(mass_ptr + size_data) = '\0';

			/// --- обновление конфигурационных файлов configunits.json--- ///
			if (UpdateFileConfigUnits(mass_data, size_data) != ResultReqest::OK)
			{
				// throw 11;
				res = ResultReqest::ERR;
			}

			try
			{
				if (!reader_config)
				{
					subscriber_->delete_datareader(reader_config);
					reader_config = nullptr;
				}
			}
			catch (...)
			{
				throw 13;
			}

			try
			{
				if (!topic_config_file)
				{
					participant_->delete_topic(topic_config_file);
					topic_config_file = nullptr;
				}
			}
			catch (...)
			{
				throw 14;
			}

			if (participant_->unregister_type("type_FileConfigDDSUnits") != ReturnCode_t::RETCODE_OK) throw 12;


		}
		catch (int& e)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error ModuleIO: Error update configunits.json";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error ModuleIO: Error update configunits.json";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			res = ResultReqest::ERR;
		}

		return res;
	};

	ResultReqest Module_IO::Init_reader_command()
	{
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (!subscriber_) throw 1;
			if (!topic_command) throw 2;

			reader_command = subscriber_->create_datareader(topic_command, DATAREADER_QOS_DEFAULT, listener_.get());
			if (!reader_command) throw 3;
		}
		catch (int& e)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error ModeleIO: Error init reader_command";
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error ModeleIO: Error init reader_command";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}
		return res;
	}

	std::string Module_IO::CreateNameStructCommand()
	{
		return "struct_commands";
	}

	std::string Module_IO::CreateNameStructAnswer()
	{
		return "struct_answer";
	}

	std::string Module_IO::CreateNameStructInfoUnits()
	{
		return "struct_infounits";
	}

	std::string Module_IO::CreateNameTopicCommand(std::string source)
	{
		return source;
	};

	std::string Module_IO::CreateNameTopicAnswer(std::string source)
	{
		return source;
	};

	std::string Module_IO::CreateNameTopicInfoUnits(std::string source)
	{
		return source + ":" + std::to_string(config_gate.IdGate);
	};

	std::string Module_IO::CreateNameTopicConfigDDSUnits()
	{
		return "DDSUnitConfig:" + std::to_string(config_gate.IdGate);
	};

	StatusModeluIO Module_IO::GetCurrentStatus()
	{
		return status.load(std::memory_order_relaxed);
	}

	ResultReqest Module_IO::UpdateFileConfigUnits(std::shared_ptr<char> data, unsigned int size)
	{
		std::string name_file = "configunits.json";
		std::fstream file;
		char* simvol_ptr = data.get();

		try 
		{
			file.open(name_file, std::fstream::in | std::fstream::out | std::fstream::trunc);
			if (!file.is_open()) throw 1;
			try { file << simvol_ptr; }
			catch (...) { throw 2; }
			file.close();
		}
		catch (int& e)
		{
			if (file.is_open()) file.close();
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error ModeleIO: Error update file configunits.json";
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch(...)
		{
			if (file.is_open()) file.close();
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error ModeleIO: Error update file configunits.json";
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	void Module_IO::SubListener::on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info)
	{};

	void Module_IO::SubListener::on_data_available(DataReader* reader)
	{};


}