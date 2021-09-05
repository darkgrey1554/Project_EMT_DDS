#pragma once

#include <iostream>

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

struct ConfigPublisher
{
	std::string IP;
	unsigned int Port;
	unsigned short Domen;
	TypeUnitTransport TypeUnit = TypeUnitTransport::ZERO;
	std::string NameListKKSOut;

	ConfigPublisher()
	{
		IP.clear();
		Port = 0;
		Domen = 0;
		TypeUnit = TypeUnitTransport::ZERO;
		NameListKKSOut.clear();
	};

	void clear()
	{
		IP.clear();
		Port = 0;
		Domen = 0;
		TypeUnit = TypeUnitTransport::ZERO;
		NameListKKSOut.clear();
	};
};

struct ConfigSubscriber
{
	std::string IP;
	unsigned int Port;
	unsigned short Domen;
	TypeUnitTransport TypeUnit = TypeUnitTransport::ZERO;
	std::string NameListKKSIn;

	ConfigSubscriber()
	{
		IP.clear();
		Port = 0;
		Domen = 0;
		TypeUnit = TypeUnitTransport::ZERO;
		NameListKKSIn.clear();
	};

	void clear()
	{
		IP.clear();
		Port = 0;
		Domen = 0;
		TypeUnit = TypeUnitTransport::ZERO;
		NameListKKSIn.clear();
	};

};