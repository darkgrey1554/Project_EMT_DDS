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

	ResultReqest Module_IO::InitModule()
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
		SampleInfo info_request;
		unsigned int size_data;
		unsigned int id;

		if (!participant_) throw 1;
		if (!subscriber_) throw 2;

		infounits = DynamicDataFactory::get_instance()->create_data(type_topic_infoddsunits);

		reader_config = subscriber_->create_datareader(topic_InfoDDSUnit, DATAREADER_QOS_DEFAULT, nullptr);
		if (reader_config == nullptr) throw 3;

		reader_config->take_next_sample(infounits.get(), &info_request);

		id = infounits->get_uint32_value(0);
		if (id != config_gate.IdGate) throw 4;

		size_data = infounits->get_uint32_value(1);
		if (size_data == 0) throw 5;

		std::shared_ptr<char> mass_data(new char[size_data], std::default_delete<char[]>());
		subscriber_->delete_datareader(reader_config);
		reader_config = nullptr;


		
		





	};



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

}