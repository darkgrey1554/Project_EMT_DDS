#pragma once
#include <structs/structs.hpp>
#include <structs/ServiceType.h>
#include <structs/interfaces.h>
#include <map>
#include <unordered_map>
#include <deque>
#include <variant>



namespace scada_ate::gate::adapter
{
	class IAdapter;
	struct IConfigAdapter;
	using IAdapter_ptr = std::shared_ptr<IAdapter>;
	using IConfigAdapter_ptr = std::shared_ptr<IConfigAdapter>;

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
		size_t offset_store = 0;
		StatusTag status = StatusTag::NONE;

		bool operator == (const InfoTag& tag)
		{
			if (this->tag != tag.tag) return false;
			if (this->id_tag != tag.id_tag) return false;
			if (this->is_array && tag.is_array)
			{
				if (this->offset != tag.offset) return false;
			}
			return true;
		};
	};

	template<typename T> struct ValueT
	{
		long long time = 0;
		T value;
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
		int id_map = 0;
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
		std::vector<ValueT<int>> data_int{};
		std::vector<ValueT<float>> data_float{};
		std::vector<ValueT<double>> data_double{};
		std::vector<ValueT<char>> data_char{};
		std::vector<ValueT<std::string>> data_str;
	};

	class IAdapter : public atech::common::IControl
	{
	protected:

		std::deque<SetTags> data;

	public:
		virtual ResultReqest InitAdapter() = 0;
		virtual ResultReqest ReadData(std::deque<SetTags>** data) = 0;
		virtual ResultReqest WriteData(const std::deque<SetTags>& data) = 0;
		virtual TypeAdapter GetTypeAdapter() = 0;
		virtual std::shared_ptr<IAnswer> GetInfo(ParamInfoAdapter param) = 0;
		
		virtual ~IAdapter() {};
	};

	std::shared_ptr<IAdapter> CreateAdapter(std::shared_ptr<IConfigAdapter> config);
}

