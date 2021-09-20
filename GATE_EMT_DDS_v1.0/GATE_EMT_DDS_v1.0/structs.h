#pragma once

#include <iostream>
#include <mutex>

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
	ERR = 1
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
	STOP,
	START,
	DEAD,
	WORK
};

enum class TypeDDSUnit
{
	Empty,
	SUBSCRIBER,
	PUBLISHER
};

struct ConfigGate
{
	unsigned int IdGate = 0;
	unsigned int Domen = 0;
	std::string TypeTransmite;
	std::string IPSubscribtion;
	unsigned int PortSubscribtion;
	std::string TopicSubscritionCommand;
	std::string TopicSubscribtionInfoConfig;
	std::string IPPublication;
	unsigned int PortPublication;
	std::string TopicPublicationAnswer;
};

/// <summary>
/// Заголовок расшаренной памяти 
/// </summary>
struct HeaderSharedMemory
{
	TypeData typedata = TypeData::ZERO;
	TypeDirection  typedirection = TypeDirection::ZERO;
	TimeUnit TimeLastWrite;
	TimeUnit TimeLastRead;
	unsigned int size_data = 0;
	unsigned int count_write = 0;
	unsigned int count_read = 0;
};

struct  ListKKSOut
{
	char KKS[10];
	unsigned int pos;
};

/*
struct ConfigPublisher
{
	std::string IP_MAIN;
	std::string IP_RESERVE;
	unsigned int Port_MAIN;
	unsigned int Port_RESERVE;
	unsigned short Domen;
	std::string NameMemory;
	std::string NameListKKSOut;

	ConfigPublisher()
	{
		IP_MAIN.clear();
		IP_RESERVE.clear();
		Port_MAIN = 0;
		Port_RESERVE = 0;
		Domen = 0;
		NameMemory.clear();
		NameListKKSOut.clear();
	};

	void clear()
	{
		IP_MAIN.clear();
		IP_RESERVE.clear();
		Port_MAIN = 0;
		Port_RESERVE = 0;
		Domen = 0;
		NameMemory.clear();
		NameListKKSOut.clear();
	};
};

struct ConfigSubscriber
{
	std::string IP_MAIN;
	std::string IP_RESERVE;
	unsigned int Port_MAIN;
	unsigned int Port_RESERVE;
	unsigned short Domen;
	std::string NameMemory;
	std::string NameListKKS;

	ConfigSubscriber()
	{
		IP_MAIN.clear();
		IP_RESERVE.clear();
		Port_MAIN = 0;
		Port_RESERVE = 0;
		Domen = 0;
		NameMemory.clear();
		NameListKKS.clear();
	};

	void clear()
	{
		IP_MAIN.clear();
		IP_RESERVE.clear();
		Port_MAIN = 0;
		Port_RESERVE = 0;
		Domen = 0;
		NameMemory.clear();
		NameListKKS.clear();
	};

};
*/

struct ConfigDDSUnit
{
	unsigned short Domen;
	TypeDDSUnit TypeUnit;
	std::string TopicName;
	std::string SMName;
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