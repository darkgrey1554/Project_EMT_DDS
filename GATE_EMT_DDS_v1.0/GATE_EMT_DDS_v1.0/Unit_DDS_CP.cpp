#include "Unit_DDS_CP.h"

namespace scada_ate
{
	namespace controller_module_io
	{
		Unit_DDS_CP::Unit_DDS_CP()
		{
			log = LoggerSpace::Logger::getpointcontact();
		}

		Unit_DDS_CP::~Unit_DDS_CP()
		{
			Clear();
			return;
		}

		ResultReqest Unit_DDS_CP::InitUnitCP(std::shared_ptr<ConfigUnitCP> config_units, std::shared_ptr<module_io::Module_IO> unit_io)
		{
			std::string helpstr;

			try 
			{
				object_control = unit_io;
				config = *(reinterpret_cast<ConfigUnitCP_DDS*>(config_units.get()));

				if (init() != ResultReqest::OK) throw 1;
			}
			catch (int& e)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: error InitUnitCP";
				log->WriteLogWARNING(helpstr.c_str(), e, 0);
				return ResultReqest::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: error InitUnitCP";
				log->WriteLogWARNING(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		}

		TypeUnitCP  Unit_DDS_CP::GetTypeUnitCP()
		{
			return TypeUnitCP::DDS;
		};

		StatusUnitCP Unit_DDS_CP::GetCurrentStatus()
		{
			return status.load(std::memory_order_relaxed);
		}

		void Unit_DDS_CP::SetCurrentStatus(StatusUnitCP value)
		{
			status.store(value, std::memory_order_relaxed);
		}

		ResultReqest Unit_DDS_CP::init()
		{
			std::string helpstr;
			ResultReqest res;
			StatusUnitCP status_ = GetCurrentStatus();

			if (status_ == StatusUnitCP::WORK)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP:Initialization already done";
				log->WriteLogWARNING(helpstr.c_str(), 0, 0);
				return ResultReqest::IGNOR;
			}

			/// --- очистка свойств --- ///
			res = clear_properties();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			/// --- создание топика команд --- ///

			res = create_type_topic_command();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			/// --- создание топика ответа --- ///

			res = create_type_topic_answer();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			res = create_dynamic_data_type();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			/// --- инициализация participant --- ///

			res = init_participant();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			/// --- инициализация subscriber --- ///

			res = init_subscriber();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			/// --- инициализация publisher --- ///

			res = init_publisher();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			/// --- регистрация типов --- ///

			res = registration_types();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			/// --- создание topics--- ///

			res = create_topics();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			/// --- инициализация reader_command --- ///

			res = Init_answerer();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			res = Init_reader_command();
			if (res != ResultReqest::OK)
			{
				SetCurrentStatus(StatusUnitCP::ERROR_INIT);
				return ResultReqest::ERR;
			}

			SetCurrentStatus(StatusUnitCP::WORK);
			return ResultReqest::OK;
		}

		ResultReqest Unit_DDS_CP::clear_properties()
		{
			ReturnCode_t res;
			std::string helpstr;

			try
			{
				if (reader_command != nullptr)
				{
					res = subscriber_->delete_datareader(reader_command);
					if (res != ReturnCode_t::RETCODE_OK) throw 1;
					reader_command = nullptr;
				}

				if (answerer != nullptr)
				{
					res = publisher_->delete_datawriter(answerer);
					if (res != ReturnCode_t::RETCODE_OK) throw 2;
					answerer = nullptr;
				}

				if (subscriber_ != nullptr)
				{
					res = participant_->delete_subscriber(subscriber_);
					if (res != ReturnCode_t::RETCODE_OK) throw 3;
					subscriber_ = nullptr;
				}

				if (publisher_ != nullptr)
				{
					res = participant_->delete_publisher(publisher_);
					if (res != ReturnCode_t::RETCODE_OK) throw 4;
					publisher_ = nullptr;
				}

				if (topic_command != nullptr)
				{
					res = participant_->delete_topic(topic_command);
					if (res != ReturnCode_t::RETCODE_OK) throw 5;
					topic_command = nullptr;
				}

				if (topic_answer != nullptr)
				{
					res = participant_->delete_topic(topic_answer);
					if (res != ReturnCode_t::RETCODE_OK) throw 6;
					topic_answer = nullptr;
				}

				if (participant_ != nullptr)
				{
					participant_->unregister_type(type_topic_command->get_name());
					participant_->unregister_type(type_topic_answer->get_name());
					res = DomainParticipantFactory::get_instance()->delete_participant(participant_);
					if (res != ReturnCode_t::RETCODE_OK) throw 7;
					participant_ = nullptr;
				}
			}
			catch (int& e_int)
			{
				helpstr.clear();
				helpstr = "Error Unit_DDS_CP : Error clear proporties";
				log->WriteLogERR(helpstr.c_str(), e_int, 0);
				return ResultReqest::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr = "Error Unit_DDS_CP : Error clear proporties";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		};

		ResultReqest Unit_DDS_CP::create_type_topic_command()
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
				helpstr = "Error Unit_DDS_CP : Error create type topic command";
				log->WriteLogERR(helpstr.c_str(), e_int, 0);
				return ResultReqest::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr = "Error Unit_DDS_CP: Error create type topic command";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		}

		ResultReqest  Unit_DDS_CP::create_type_topic_answer()
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
				helpstr = "Error Unit_DDS_CP: Error create type topic answer";
				log->WriteLogERR(helpstr.c_str(), e_int, 0);
				return ResultReqest::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr = "Error Unit_DDS_CP: Error create type topic answer";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		};

		ResultReqest  Unit_DDS_CP::create_dynamic_data_type()
		{
			std::string helpstr;
			ResultReqest res = ResultReqest::OK;

			try 
			{
				try 
				{
					data_command = DynamicDataFactory::get_instance()->create_data(type_topic_command);
				}
				catch (...)
				{
					throw 1;
				}

				try
				{
					data_answer = DynamicDataFactory::get_instance()->create_data(type_topic_answer);
				}
				catch (...)
				{
					throw 2;
				}
			}
			catch (int& e)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error create of participant";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				res = ResultReqest::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error create of participant";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				res = ResultReqest::ERR;
			}

			return res;
		};

		ResultReqest Unit_DDS_CP::init_participant()
		{
			std::string helpstr;

			/// --- инициализация транспорта --- ///
			///--------------------------------------------///
			//////////////////////////////////////////////////

			/// --- инициализация participant --- /// 	

			try
			{
				participant_ =
					DomainParticipantFactory::get_instance()->create_participant(config.domen, PARTICIPANT_QOS_DEFAULT, nullptr);
				if (!participant_) throw 2;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error create of participant";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		};

		ResultReqest Unit_DDS_CP::init_subscriber()
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
				helpstr += "Error Unit_DDS_CP: Error create of subscriber";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		};

		ResultReqest Unit_DDS_CP::init_publisher()
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
				helpstr += "Error Unit_DDS_CP: Error create of publisher";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		};

		ResultReqest Unit_DDS_CP::registration_types()
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

			}
			catch (int& e)
			{
				std::string helpstr;
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error registration of type";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				return ResultReqest::ERR;
			}
			catch (...)
			{
				std::string helpstr;
				helpstr.clear();
				helpstr += "Error DDSUnit: Error registration of type";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		};

		ResultReqest Unit_DDS_CP::create_topics()
		{
			std::string helpstr;
			try
			{
				topic_command = participant_->
					create_topic(CreateNameTopicCommand(config.name_topiccommand), CreateNameStructCommand(), TOPIC_QOS_DEFAULT);
				if (!topic_command) throw 1;

				topic_answer = participant_->
					create_topic(CreateNameTopicAnswer(config.name_topicanswer), CreateNameStructAnswer(), TOPIC_QOS_DEFAULT);
				if (!topic_answer) throw 2;
			}
			catch (...)
			{
				std::string helpstr;
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error create topics";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}

			return ResultReqest::OK;
		};

		ResultReqest Unit_DDS_CP::Init_answerer()
		{
			ResultReqest res = ResultReqest::OK;

			try
			{
				if (!publisher_) throw 1;
				if (!topic_answer) throw 2;

				answerer = publisher_->create_datawriter(topic_answer, DATAWRITER_QOS_DEFAULT, nullptr);
				if (!answerer) throw 3;
			}
			catch (int& e)
			{
				std::string helpstr;
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error Init_answerer";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				return ResultReqest::ERR;
			}
			catch (...)
			{
				std::string helpstr;
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error Init_answerer";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}
			return res;
		}

		ResultReqest Unit_DDS_CP::Init_reader_command()
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
				helpstr += "Error Unit_DDS_CP: Error init reader_command";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				return ResultReqest::ERR;
			}
			catch (...)
			{
				std::string helpstr;
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error init reader_command";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ResultReqest::ERR;
			}
			return res;
		}

		void Unit_DDS_CP::SubListener::on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info)
		{};

		void Unit_DDS_CP::SubListener::on_data_available(DataReader* reader)
		{
			SampleInfo info;
			eprosima::fastrtps::types::DynamicData* array = nullptr;
			std::string helpstr;
			ListUsedAnswer result;

			if (reader->take_next_sample(master->data_command.get(), &info) != ReturnCode_t::RETCODE_OK) return;
			if (!info.valid_data) return;
			if (master->data_command->get_uint32_value(0) != master->config.id_gate) return;

			result = master->ProcessingOfCommand((ListUsedCommand)master->data_command->get_uint32_value(1));

			master->data_answer->set_uint32_value(master->config.id_gate, 0);
			master->data_answer->set_uint32_value(master->data_command->get_uint32_value(1), 1);
			master->data_answer->set_uint32_value((unsigned int)result, 2);
			master->data_answer->set_uint32_value(0, 3);

			master->answerer->write(master->data_answer.get());

		};

		ListUsedAnswer Unit_DDS_CP::ProcessingOfCommand(ListUsedCommand command)
		{
			if (command == ListUsedCommand::StopTransfer_ModuleIO)
			{
				return Processing_StopTransfer_ModuleIO();
			}

			if (command == ListUsedCommand::StartTransfer_ModuleIO)
			{
				return Processing_StartTransfer_ModuleIO();
			}

			if (command == ListUsedCommand::Restart_ModuleIO)
			{
				return Processing_Restart_ModuleIO();
			}

			if (command == ListUsedCommand::UpdateUnits_ModuleIO)
			{
				return Processing_UpdateUnits_ModuleIO();
			}

			if (command == ListUsedCommand::Apply_UpdateUnits_ModuleIO)
			{
				return Processing_Apply_UpdeteUnits_ModuleIO();
			}

			if (command == ListUsedCommand::Terminate_Gate)
			{
				return Processing_Terminate_Gate();
			}

			return ListUsedAnswer::NOT_SUPPORTED;
		}

		ListUsedAnswer Unit_DDS_CP::Processing_StopTransfer_ModuleIO()
		{
			std::shared_ptr<module_io::Module_IO> unit;
			ListUsedAnswer result = ListUsedAnswer::ERR;
			ResultReqest result_request;
			std::string helpstr;

			try
			{
				if (!(unit = object_control.lock())) throw 1;

				result_request = unit->StopTransfer();
				result = ResultRequestToAnswer(result_request);

			}
			catch (int& e)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command StopTransfer_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				return ListUsedAnswer::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command StopTransfer_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ListUsedAnswer::ERR;
			}

			return result;
		}

		ListUsedAnswer Unit_DDS_CP::Processing_StartTransfer_ModuleIO()
		{
			std::shared_ptr<module_io::Module_IO> unit;
			ListUsedAnswer result = ListUsedAnswer::ERR;
			std::string helpstr;
			ResultReqest result_request;

			try
			{
				if (!(unit = object_control.lock())) throw 1;
				result_request = unit->StartTransfer();
				result = ResultRequestToAnswer(result_request);
			}
			catch (int& e)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command StartTransfer_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				return ListUsedAnswer::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command StartTransfer_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ListUsedAnswer::ERR;
			}

			return result;
		}

		ListUsedAnswer Unit_DDS_CP::Processing_Restart_ModuleIO()
		{
			std::shared_ptr<module_io::Module_IO> unit;
			ListUsedAnswer result = ListUsedAnswer::ERR;
			ResultReqest result_request;
			std::string helpstr;

			try
			{
				if (!(unit = object_control.lock())) throw 1;
				result_request = unit->ReInitModule();
				result = ResultRequestToAnswer(result_request);
			}
			catch (int& e)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command Restart_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				return ListUsedAnswer::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command Restart_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ListUsedAnswer::ERR;
			}

			return result;
		};

		ListUsedAnswer Unit_DDS_CP::Processing_UpdateUnits_ModuleIO()
		{
			std::shared_ptr<module_io::Module_IO> unit;
			ListUsedAnswer result = ListUsedAnswer::ERR;
			ResultReqest result_request;
			std::string helpstr;

			try
			{
				if (!(unit = object_control.lock())) throw 1;
				result_request = unit->UpdateConfigDDSUnits();
				result = ResultRequestToAnswer(result_request);
			}
			catch (int& e)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command UpdateUnits_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				return ListUsedAnswer::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command UpdateUnits_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ListUsedAnswer::ERR;
			}

			return result;
		}

		ListUsedAnswer Unit_DDS_CP::Processing_Apply_UpdeteUnits_ModuleIO()
		{
			std::shared_ptr<module_io::Module_IO> unit;
			ListUsedAnswer result = ListUsedAnswer::ERR;
			ResultReqest result_request;
			std::string helpstr;

			try
			{
				if (!(unit = object_control.lock())) throw 1;
				result_request = unit->ApplyUpdateDDSUnits();
				result = ResultRequestToAnswer(result_request);
			}
			catch (int& e)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command Apply_UpdeteUnits_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				return ListUsedAnswer::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: Error command Apply_UpdeteUnits_ModuleIO ";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				return ListUsedAnswer::ERR;
			}

			return result;
		};

		ListUsedAnswer Unit_DDS_CP::Processing_Terminate_Gate()
		{
			SetGlobalStatus(GlobalStatus::END);
			return ListUsedAnswer::OK;
		}

		std::string Unit_DDS_CP::CreateNameStructCommand()
		{
			return "struct_commands";
		}

		std::string Unit_DDS_CP::CreateNameStructAnswer()
		{
			return "struct_answer";
		}		

		std::string Unit_DDS_CP::CreateNameTopicCommand(std::string source)
		{
			return source;
		};

		std::string Unit_DDS_CP::CreateNameTopicAnswer(std::string source)
		{
			return source;
		};

		ListUsedAnswer Unit_DDS_CP::ResultRequestToAnswer(ResultReqest value)
		{
			switch (value)
			{
			case ResultReqest::ERR:
				return ListUsedAnswer::ERR;
				break;
			case ResultReqest::OK:
				return ListUsedAnswer::OK;
				break;
			case ResultReqest::IGNOR:
				return ListUsedAnswer::IGNOR;
				break;
			default:
				return ListUsedAnswer::ERR;
			}
		}

		ResultReqest Unit_DDS_CP::Clear()
		{
			ResultReqest res = ResultReqest::OK;
			std::string helpstr;

			try
			{
				if (reader_command != nullptr)
				{
					subscriber_->delete_datareader(reader_command);
					reader_command = nullptr;
				}

				if (answerer != nullptr)
				{
					publisher_->delete_datawriter(answerer);
					answerer = nullptr;
				}

				if (topic_command != nullptr)
				{
					participant_->delete_topic(topic_command);
					topic_command = nullptr;
				}

				if (topic_answer != nullptr)
				{
					participant_->delete_topic(topic_answer);
					topic_answer = nullptr;
				}

				if (participant_ != nullptr)
				{
					participant_->unregister_type(type_topic_command->get_name());
					participant_->unregister_type(type_topic_answer->get_name());
					DomainParticipantFactory::get_instance()->delete_participant(participant_);
					participant_ = nullptr;
				}
			}
			catch (int& e)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: error Clear: ";
				log->WriteLogERR(helpstr.c_str(), e, 0);
				res = ResultReqest::ERR;
			}
			catch (...)
			{
				helpstr.clear();
				helpstr += "Error Unit_DDS_CP: error Clear: ";
				log->WriteLogERR(helpstr.c_str(), 0, 0);
				res = ResultReqest::ERR;
			}


			

			return res;
		}
	}
}