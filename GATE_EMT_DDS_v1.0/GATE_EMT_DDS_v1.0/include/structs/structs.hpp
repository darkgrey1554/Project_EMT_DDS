#pragma once

#include <iostream>
#include <mutex>

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
	ERR = 1,
	IGNOR = 2
};


enum class TypeValue
{
	INPUT,
	OUTPUT
};


/// <summary>
/// ������������: ����������� �������� ������ 
/// </summary>


enum class TypeUnitTransport
{
	ZERO,
	MAIN,
	RESERVER
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

