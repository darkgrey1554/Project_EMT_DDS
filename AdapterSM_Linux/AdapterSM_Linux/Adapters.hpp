#pragma once

//#include "structs/structs.hpp"
#include <map>
#include <unordered_map>
#include <deque>
#include <vector>
#include <memory>

enum class ResultReqest
{
	OK = 0,
	ERR = 1,
	IGNOR = 2
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

namespace scada_ate::gate::adapter
{
	class IAdapter;

	using IAdapter_ptr = std::shared_ptr<IAdapter>;

	enum class TypeAdapter
	{
		SharedMemory,
		DDS,
		DTS,
		OPC_UA,
		SMTP,
		TCP,
		Null
	};

	enum class TypeValue
	{
		INT,
		FLOAT,
		DOUBLE,
		CHAR,
		STRING
	};

	enum class TypeRegistration
	{
		RECIVE,
		UPDATE,
		DELTA
	};

	struct InfoTag
	{
		std::string tag;
		int id_tag;
		bool is_array;
		size_t offset;
		TypeValue type;
		unsigned int mask;
	};

	struct InfoTagHash {
		std::size_t operator()(const InfoTag& k) const
		{
			std::size_t h1 = std::hash<std::string>{}(k.tag);
			std::size_t h2 = std::hash<size_t>{}(k.offset);
			std::size_t h3 = std::hash<int>{}(k.id_tag);
			return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
		}
	};

	struct InfoTagEqual {
		bool operator()(const InfoTag& lhs, const InfoTag& rhs) const
		{
			if (lhs.id_tag != rhs.id_tag) return false;
			if (lhs.offset != rhs.offset) return false;
			if (lhs.tag != rhs.tag) return false;
			return true;
		}
	};

	struct ValueInt
	{
		long long time = 0;
		int value = 0;
		char quality = 0;
	};

	struct ValueFloat
	{
		long long time;
		float value;
		char quality;
	};

	struct ValueDouble
	{
		long long time;
		double value;
		char quality;
	};

	struct ValueChar
	{
		long long time;
		char value;
		char quality;
	};

	struct ValueString
	{
		long long time;
		std::string value;
		char quality;
	};

	struct LinkTags
	{
		InfoTag source;
		InfoTag target;
		TypeRegistration type_registration;
		double delta;
	};

	struct IConfigAdapter
	{
		TypeAdapter type_adapter = TypeAdapter::Null;
		std::vector<InfoTag> vec_tags_source;
		std::vector<LinkTags> vec_link_tags;
		virtual ~IConfigAdapter() {};
	};

	struct IAnswer
	{
		TypeAdapter type_adapter;
		ParamInfoAdapter param;
		ResultReqest result;
	};	

	struct SetTags
	{
		long long time_source;
		std::unordered_map<InfoTag, ValueInt , InfoTagHash, InfoTagEqual> map_int_data;
		std::unordered_map<InfoTag, ValueFloat, InfoTagHash, InfoTagEqual> map_float_data;
		std::unordered_map<InfoTag, ValueDouble, InfoTagHash, InfoTagEqual> map_double_data;
		std::unordered_map<InfoTag, ValueChar, InfoTagHash, InfoTagEqual> map_char_data;
		std::unordered_map<InfoTag, ValueString, InfoTagHash, InfoTagEqual> map_str_data;
	};

	class IAdapter
	{
	protected:

		std::deque<SetTags> data;

	public:
		virtual ResultReqest InitAdapter() = 0;
		virtual ResultReqest ReadData(std::deque<SetTags>** data) = 0;
		virtual ResultReqest WriteData(const std::deque<SetTags>& data) = 0;
		virtual TypeAdapter GetTypeAdapter() = 0;
		virtual StatusAdapter GetStatusAdapter() = 0;
		virtual std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param) = 0;
		
		virtual ~IAdapter() {};
	};

	std::shared_ptr<IAdapter> CreateAdapter(std::shared_ptr<IConfigAdapter> config);
}

