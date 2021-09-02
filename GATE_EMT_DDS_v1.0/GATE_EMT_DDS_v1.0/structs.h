#pragma once

#include <iostream>

/// <summary>
///  ��� �����
/// </summary>
enum class Type_Gate
{
	DDS = 0,
	DTS = 1
};


/// <summary>
/// ������������: ��������� ���������� �������
/// </summary>
enum class ResultReqest
{
	OK = 0,
	ERROR = 1
};

enum class TypeData
{
	ZERO,
	ANALOG,
	DISCRETE,
	BINAR
};

/// <summary>
/// ��������� ��� �������� ������� � ����������� ������
/// </summary>
struct TimeUnit
{
	unsigned char h = 0;
	unsigned char m = 0;
	unsigned char s = 0;
	unsigned short ms = 0;
};

/// <summary>
/// ������������: ����������� �������� ������ 
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

/// <summary>
/// ��������� ����������� ������ 
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


struct ConfigPublisher
{
	std::string IP;
	unsigned int PORT;
	unsigned char Domen;
};

struct ConfigSubscriber
{
	std::string IP;
	unsigned int PORT;
	unsigned char Domen;

};