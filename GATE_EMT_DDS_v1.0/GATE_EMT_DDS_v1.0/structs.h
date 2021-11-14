#pragma once

#include <iostream>
#include <mutex>
#include <logger.h>

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

enum class TypeData
{
	ZERO,
	ANALOG,
	DISCRETE,
	BINAR
};

enum class TypeValue
{
	INPUT,
	OUTPUT
};

/// <summary>
/// структура для хранения времени в расшаренной памяти
/// </summary>
struct TimeUnit
{
	unsigned char h = 0;
	unsigned char m = 0;
	unsigned char s = 0;
	unsigned short ms = 0;
};

/// <summary>
/// перечисление: направление передачи данных 
/// </summary>
enum class TypeDirection
{
	ZERO,
	EMTtoDDS,
	EMTfromDDS
};

enum class Typetransport
{
	TCPv4,
	UDPv4
};

enum class TypeUnitTransport
{
	ZERO,
	MAIN,
	RESERVER
};

/// <summary>
/// команды по управлению ДДС subscriber, publisher
/// </summary>
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

enum class TypeDDSUnit
{
	Empty,
	SUBSCRIBER,
	PUBLISHER
};

enum class TypeTransmiter
{
	TCP,
	UDP,
	Broadcast
};

enum class TypeAdapter
{
	SharedMemory,
	DDS,
	DTS,
	OPC_UA,
	SMTP,
	Null
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

enum class CommandListenerSubscriber
{
	NONE,
	START,
	STOP
};

struct BaseAnswer
{
	TypeAdapter typeadapter;
	ParamInfoAdapter param;
	ResultReqest result;

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



/// <summary>
/// Заголовок расшаренной памяти 
/// </summary>
struct HeaderSharedMemory
{
	TypeData typedata = TypeData::ZERO;
	TimeUnit TimeLastWrite;
	TimeUnit TimeLastRead;
	unsigned int size_data = 0;
	unsigned long count_write = 0;
	unsigned long count_read = 0;
};

struct  ListKKSOut
{
	char KKS[10];
	unsigned int pos;
};

struct ConfigDDSUnit
{
	unsigned short Domen;
	TypeDDSUnit TypeUnit;
	TypeTransmiter Transmiter;
	TypeAdapter Adapter;
	std::string PointName;
	TypeData Typedata;
	unsigned int Size;
	unsigned int Frequency;
	std::string IP_MAIN;
	std::string IP_RESERVE;
	unsigned int Port_MAIN;
	unsigned int Port_RESERVE;
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

struct InfoDDSUnit
{
	ControlDDSUnit control;
	ConfigDDSUnit config;
};

struct KKSUnit
{
	std::string KKS;
	unsigned int position;
};

struct ConfigAdapter
{
	TypeAdapter type_adapter;
};

struct ConfigSharedMemoryAdapter : public ConfigAdapter
{
	TypeData DataType;
	unsigned size;
	std::string NameMemory;
};


struct HeaderDataAnswerSM : public BaseAnswer
{
	HeaderSharedMemory header;
};

struct ConfigModule_IO_DDS
{
	unsigned int IdGate = 0;
	unsigned int Domen = 0;
	TypeTransmiter TypeTransmite;
	std::string IPSubscribtion;
	unsigned int PortSubscribtion;
	std::string TopicSubscritionCommand;
	std::string TopicSubscribtionInfoConfig;
	std::string IPPublication;
	unsigned int PortPublication;
	std::string TopicPublicationAnswer;
};

struct ConfigContreller_TCP
{
	std::string IP;
	unsigned int Port = 0;
};

struct ConfigContreller_DDS
{
	unsigned int domen = 0;
	TypeTransmiter type_transmiter;
	std::string topic_command;
	std::string topic_answer;
	std::string ip_base;
	std::string ip_reserve;
	unsigned int port_base;
	unsigned int port_reserve;
};

struct ConfigModule_IO
{
	unsigned int IdGate = 0;
	unsigned int domen = 0;
	TypeTransmiter type_transmiter;
	std::string topic_info;
	std::string ip_base;
	std::string ip_reserve;
	unsigned int port_base;
	unsigned int port_reserve;
};
