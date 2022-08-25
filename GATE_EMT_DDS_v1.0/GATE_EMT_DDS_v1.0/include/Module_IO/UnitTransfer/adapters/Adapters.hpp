#pragma once
#include <structs/structs.hpp>
#include <map>
#include <unordered_map>
#include <deque>
#include <variant>



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

	enum class StatusTag
	{
		NONE,
		NOTDETECTED,
		BADDIMENSION,
		BADINDEX,
		BADTYPE,
		OK
	};

	enum class StatusAdapter
	{
		OK,
		ERROR_INIT,
		INITIALIZATION,
		CRASH,
		Null
	};

	struct InfoTag
	{
		std::string tag;
		int id_tag = 0;
		bool is_array = false;
		size_t offset = 0;
		TypeValue type;
		unsigned int mask = 0;
		StatusTag status = StatusTag::NONE;
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

	struct Value
	{
		long long time = 0;
		std::variant<int, float, double, char, std::string> value;
		char quality = 0;
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
		int id_adapter = 0;
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
		std::unordered_map<InfoTag, Value , InfoTagHash, InfoTagEqual> map_data;
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

