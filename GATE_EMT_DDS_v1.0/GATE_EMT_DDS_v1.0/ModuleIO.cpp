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
		ResultReqest res = ResultReqest::OK;

		/// --- чтение конфигурации --- /// 
		res = reader_config->ReadConfigGATE(config_gate);
		if (res != ResultReqest::OK) throw 1;

		/// --- инициализация типа топика команд --- ///

		res = create_type_topic_command();
		if (res != ResultReqest::OK) throw 2;

		/// --- инициализация типа топика ответа --- ///

		res = create_type_topic_answer();
		if (res != ResultReqest::OK) throw 3;

		/// --- инициализация типа топика конфигурации dds units --- ///

		res = create_type_topic_infoddsunits();
		if (res != ResultReqest::OK) throw 4;

		/// --- инициализация participant --- ///



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
				helpstr += CreateNameTopicCommand(config_gate.TopicSubscritionCommand);
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
		catch (const int& e_int)
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
				helpstr += CreateNameTopicAnswer(config_gate.TopicPublicationAnswer);
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
				helpstr += CreateNameTopicInfoUnits(config_gate.TopicSubscribtionInfoConfig);
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

}