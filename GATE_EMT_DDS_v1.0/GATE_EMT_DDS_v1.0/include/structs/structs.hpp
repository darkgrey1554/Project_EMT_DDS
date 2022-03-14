#pragma once

#include <iostream>
#include <mutex>
#include <logger_crossplatform/logger.h>

/// <summary>
///  тип шлюза
/// </summary>





enum class Type_Gate
{
	DDS = 0,
	DTS = 1
};


/// <summary>
/// перечисление: результат выполнения функции
/// </summary>
enum class ResultReqest
{
	OK = 0,
	ERR = 1,
	IGNOR = 2
};


enum class TypeValue
{
	INPUT,
	OUTPUT
};


/// <summary>
/// перечисление: направление передачи данных 
/// </summary>


enum class TypeUnitTransport
{
	ZERO,
	MAIN,
	RESERVER
};


enum class StatusAdapter
{
	OK,
	ERROR_INIT,
	INITIALIZATION,
	Null
};

enum class ParamInfoAdapter
{
	Type,
	Config,
	HeaderData
};

enum class StatusThreadDSSUnit
{
	NONE,
	WORK,
	TERMINATE,
	FAIL
};

enum class StatusModeluIO
{
	WORK,
	STOP,
	ERROR_INIT,
	INITIALIZATION,
	Null
};


struct ConfigGate
{
	unsigned int IdGate = 0;
};

struct ConfigLogger
{
	std::string LogName;
	std::string SysLogName;
	LoggerSpace::LogMode LogMode;
	LoggerSpace::Status StatusLog;
	LoggerSpace::Status StatusSysLog;
	unsigned SizeLogFile;
};

struct ConfigManager
{
	std::string IP;
	unsigned int Port;
};


/*struct ConfigModule_IO
{
	unsigned int IdGate = 0;
	unsigned int domen = 0;
	TypeTransmiter type_transmiter;
	std::string topic_info;
	std::string ip_base;
	std::string ip_reserve;
	unsigned int port_base;
	unsigned int port_reserve;
};*/
