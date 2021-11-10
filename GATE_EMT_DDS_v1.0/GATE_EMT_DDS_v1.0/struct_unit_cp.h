#pragma once
#include <string>


namespace scada_ate
{
	namespace controller_module_io
	{
		enum class StatusUnitCP
		{
			ERROR_INIT,
			WORK,
			EMPTY
		};

		enum class TypeUnitCP
		{
			DDS,
			TCP
		};

		enum class TypeTransmite
		{
			BroadCast,
			TCP,
			UDP
		};

		enum class GlobalStatus
		{
			END,
			WORK
		};

		struct ConfigUnitCP
		{
			TypeUnitCP type_unit;
			unsigned int id_gate;
		};

		struct ConfigUnitCP_DDS : public ConfigUnitCP
		{
			unsigned int domen;
			TypeTransmite type_transmite;
			std::string ip_subscriber;
			std::string ip_publisher;
			unsigned int port_subscriber;
			unsigned int port_publisher;
			std::string name_topiccommand;
			std::string name_topicanswer;
		};

		struct ConfigUnitCP_TCP : public ConfigUnitCP
		{
			unsigned int port;
			std::string ip;
		};

		enum class ListUsedCommand
		{
			StopTransfer_ModuleIO = 0,
			StartTransfer_ModuleIO = 1,
			Restart_ModuleIO = 2,
			UpdateUnits_ModuleIO = 3,

			Terminate_Gate = 10
		};

		enum class ListUsedAnswer
		{
			OK = 0,
			ERR = 1,
			IGNOR =2,
			NOT_SUPPORTED = 3,
		};


	}
}

