#pragma once
#include <Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include <structs/TimeConverter.hpp>
#include <LoggerScada.h>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <ddsformat/TypeTopics.h>
#include <structs/FactoryDds.h>


namespace _dds = eprosima::fastdds::dds;

namespace scada_ate::gate::adapter::dds
{
	enum class TypeTransfer
	{
		PUBLISHER,
		SUBSCRIBER
	};	

	enum class TypeDDSData
	{
		DDSData,
		DDSDataEx,
		DDSAlarm,
		DDSAlarmEx
	};

	struct ConfigAdapterDDS : public IConfigAdapter
	{
		std::string topic_name = "";
		TypeTransfer type_transfer = TypeTransfer::SUBSCRIBER;
		TypeDDSData type_data;
		std::string str_config_ddslayer;
	};

	template<typename T>
	class AdapterDDS : public IAdapter
	{
		
		ConfigAdapterDDS config;
		std::mutex mutex_init;
		std::atomic<atech::common::Status> current_status = atech::common::Status::NONE;
		atech::logger::ILoggerScada_ptr log; /// ������

		std::shared_ptr<atech::srv::io::FactoryDDS> _factory_dds = nullptr;
		_dds::DataReader* _datareader = nullptr;
		_dds::DataWriter* _datawriter = nullptr;
		std::shared_ptr<T> _data_dds;

		size_t size_vector_int_settags = 0;
		size_t size_vector_float_settags = 0;
		size_t size_vector_double_settags = 0;
		size_t size_vector_char_settags = 0;
		size_t size_vector_str_settags = 0;

		std::unordered_map<unsigned int, InfoTag> map_infotag_to_idtag{};
		SetTags _storage;

		size_t count_read_packets = 10;

		ResultReqest init_participant();
		ResultReqest registration_type();
		ResultReqest init_topic();
		ResultReqest init_transport();
		ResultReqest init_publisher();
		ResultReqest init_subscriber();
		ResultReqest init_zero_buffer();
		ResultReqest init_map_infotags_to_idtag();
		
		ResultReqest init_StoreLastValue();
		atech::srv::io::TypeTopic typetopic_to_typeDataDDS(const TypeDDSData& type_dds);
		
		ResultReqest init_adapter();
		void destroy();
		
		std::string get_name_participant_profile();
		std::string get_name_topic();
		std::string get_name_topic_profile();
		std::string get_typetopic(const TypeDDSData& type_ddsdata);
		std::string get_name_publisher_profile();
		std::string get_name_subscriber_profile();
		std::string get_name_datawriter_profile();
		std::string get_name_datareader_profile();

		ResultReqest write_to_deque(DDSData* buf, size_t& count);
		ResultReqest write_to_deque(DDSDataEx* buf, size_t& count);
		ResultReqest write_to_deque(DDSAlarm* buf, size_t& count);
		ResultReqest write_to_deque(DDSAlarmEx* buf, size_t& count);

		ResultReqest create_dds_data(const SetTags& in_data);

		void clear_out_buffer(T* buf);
		/*void clear_out_buffer(DDSData* buf);
		void clear_out_buffer(DDSDataEx* buf);
		void clear_out_buffer(DDSAlarm* buf);
		void clear_out_buffer(DDSAlarmEx* buf);*/
	
		void update_head(const long long& time_sourse, T* buf);
		//void update_head(const long long& time_sourse, DDSDataEx* buf);
		//void update_head(const long long& time_sourse, DDSAlarm* buf);
		//void update_head(const long long& time_sourse, DDSAlarmEx* buf);

		ResultReqest init_buffer(T* buf);
		//ResultReqest init_buffer(DDSData* buf);
		//ResultReqest init_buffer(DDSDataEx* buf);
		//ResultReqest init_buffer(DDSAlarm* buf);
		//ResultReqest init_buffer(DDSAlarmEx* buf);

		void set_data(const ValueT<int>& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueT<int>& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueT<int>& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueT<int>& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void set_data(const ValueT<float>& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueT<float>& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueT<float>& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueT<float>& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void set_data(const ValueT<double>& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueT<double>& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueT<double>& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueT<double>& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void set_data(const ValueT<char>& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueT<char>& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueT<char>& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueT<char>& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void set_data(const ValueT<std::string>& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueT<std::string>& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueT<std::string>& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueT<std::string>& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void copy_str_to_vchar(std::vector<char>& vchar, const std::string& str);
		bool is_equal_str_with_vchar(std::vector<char>& vchar, const std::string& str);
		int demask(const int& value, int mask_source, const int& value_target, const int& mask_target);

	public:

		ResultReqest InitAdapter() override;
		ResultReqest ReadData(std::deque<SetTags>** _data) override;
		ResultReqest WriteData(const std::deque<SetTags>& _data) override;
		TypeAdapter GetTypeAdapter() override;
		ResultReqest GetStatus(std::deque<std::pair<uint32_t, atech::common::Status>>& st ,uint32_t id = 0) override;
		uint32_t GetId() override;
		ResultReqest Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;

		std::shared_ptr<IAnswer> GetInfo(ParamInfoAdapter param) override;

		AdapterDDS(std::shared_ptr<IConfigAdapter> config);
		~AdapterDDS();
	};

	template<typename T> AdapterDDS<T>::AdapterDDS(std::shared_ptr<IConfigAdapter> config)
	{
		std::shared_ptr<ConfigAdapterDDS> config_point = std::reinterpret_pointer_cast<ConfigAdapterDDS>(config);

		if (config_point != nullptr && config_point->type_adapter == TypeAdapter::DDS)
		{
			this->config = *config_point;
		}

		log = std::make_shared<atech::logger::LoggerScadaSpdDds>();
	};

	template<class T> AdapterDDS<T>::~AdapterDDS()
	{
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		destroy();
	};

	template<typename T> ResultReqest  AdapterDDS<T>::InitAdapter()
	{
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		return init_adapter();
	}

	template<typename T> ResultReqest  AdapterDDS<T>::init_adapter()
	{
		ResultReqest result = ResultReqest::OK;

		/// --- guard from repeated usage --- ///

		atech::common::Status status = current_status.load(std::memory_order::memory_order_relaxed);
		if (status == atech::common::Status::INIT || status == atech::common::Status::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}
		current_status.store(atech::common::Status::INIT, std::memory_order_relaxed);

		try
		{
			log->Debug("AdapterDDS id-{}: Start init", config.id_adapter);

			if (config.type_adapter != TypeAdapter::DDS) throw 1;

			if (!_factory_dds)
			{
				_factory_dds = atech::srv::io::FactoryDDS::get_instance();
				if (!_factory_dds) throw 2;
			}

			if (_factory_dds->init_dds() != ResultReqest::OK) throw 3;

			if (init_topic() != ResultReqest::OK) throw 4;

			if (init_transport() != ResultReqest::OK) throw 5;

			if (init_map_infotags_to_idtag() != ResultReqest::OK) throw 9;

			if (init_zero_buffer() != ResultReqest::OK) throw 6;

			_data_dds = std::make_shared<T>(T());

			if (init_buffer(_data_dds.get()) != ResultReqest::OK) throw 7;

			if (init_StoreLastValue() != ResultReqest::OK) throw 8;

			current_status.store(atech::common::Status::OK);

			log->Info("AdapterDDS id - {}: Init done", config.id_adapter);
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error Init : error: {}", this->config.id_adapter, e);
			current_status.store(atech::common::Status::ERROR_INIT);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Init : error: {}", this->config.id_adapter, 0);
			current_status.store(atech::common::Status::ERROR_INIT);
			result = ResultReqest::ERR;
		}

		return result;
	};

	template<typename T> ResultReqest AdapterDDS<T>::init_topic()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (_factory_dds->registration_topic(config.topic_name, typetopic_to_typeDataDDS(config.type_data)) != ResultReqest::OK)
				throw 1;
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error init topic : error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error init tipic : error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_transport()
	{
		ResultReqest result = ResultReqest::OK;

		switch (config.type_transfer)
		{
		case TypeTransfer::SUBSCRIBER:
			result = init_subscriber();
			break;
		case TypeTransfer::PUBLISHER:
			result = init_publisher();
			break;
		default:
			result = ResultReqest::ERR;
			break;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_publisher()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			_datawriter = _factory_dds->get_datawriter(config.topic_name);
			if (!_datawriter) throw 1;
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Init publisher : error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Init publisher: error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_subscriber()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			_datareader = _factory_dds->get_datareader(config.topic_name);
			if (!_datareader) throw 1;
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Init subscriber : error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Init subscriber : error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_zero_buffer()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			for (InfoTag& tag : this->config.vec_tags_source)
			{
				if (tag.type == TypeValue::INT)
				{
					size_vector_int_settags = std::max(size_vector_int_settags, tag.offset_store + 1);
				}
				else if (tag.type == TypeValue::FLOAT)
				{
					size_vector_float_settags = std::max(size_vector_float_settags, tag.offset_store + 1);
				}
				else if (tag.type == TypeValue::DOUBLE)
				{
					size_vector_double_settags = std::max(size_vector_double_settags, tag.offset_store + 1);
				}
				else if (tag.type == TypeValue::CHAR)
				{
					size_vector_char_settags = std::max(size_vector_char_settags, tag.offset_store + 1);
				}
				else if (tag.type == TypeValue::STRING)
				{
					size_vector_str_settags = std::max(size_vector_str_settags, tag.offset_store + 1);
				}
			}

			data.resize(1);
			data[0].data_int.resize(size_vector_int_settags);
			data[0].data_float.resize(size_vector_float_settags);
			data[0].data_double.resize(size_vector_double_settags);
			data[0].data_char.resize(size_vector_char_settags);
			data[0].data_str.resize(size_vector_str_settags);


		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error init tempalte settags : error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Error init tempalte settags : error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}
		

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_map_infotags_to_idtag()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			for (InfoTag& tag : this->config.vec_tags_source)
			{
				map_infotag_to_idtag[tag.id_tag] = tag;
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error Init map of infotag to idtag : error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Init : error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_StoreLastValue()
	{
		ResultReqest result = ResultReqest::OK;

		if (config.type_data == TypeDDSData::DDSDataEx || config.type_data == TypeDDSData::DDSAlarmEx)
		{
			size_t size_int_vector = 0;
			size_t size_float_vector = 0;
			size_t size_double_vector = 0;
			size_t size_char_vector = 0;
			size_t size_str_vector = 0;

			for (LinkTags& link : this->config.vec_link_tags)
			{
				if (link.target.type == TypeValue::INT)
				{
					size_int_vector = std::max(size_int_vector, link.target.offset_store);
				}
				else if (link.target.type == TypeValue::FLOAT)
				{
					size_float_vector = std::max(size_float_vector, link.target.offset_store);
				}
				else if (link.target.type == TypeValue::DOUBLE)
				{
					size_double_vector = std::max(size_double_vector, link.target.offset_store);
				}
				else if (link.target.type == TypeValue::CHAR)
				{
					size_char_vector = std::max(size_char_vector, link.target.offset_store);
				}
				else if (link.target.type == TypeValue::STRING)
				{
					size_str_vector = std::max(size_str_vector, link.target.offset_store);
				}
			}

			_storage.data_int.resize(size_int_vector);
			_storage.data_float.resize(size_float_vector);
			_storage.data_double.resize(size_double_vector);
			_storage.data_char.resize(size_char_vector);
			_storage.data_str.resize(size_str_vector);
		}			

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::ReadData(std::deque<SetTags>** _data)
	{
		ResultReqest result = ResultReqest::OK;

		if (current_status.load() != atech::common::Status::OK)
		{
			result = ResultReqest::IGNOR;
			return result;
		}

		if (config.type_transfer != TypeTransfer::SUBSCRIBER)
		{
			log->Debug("AdapterDDS id-{}: Error ReadData : not supported", this->config.id_adapter);
			return ResultReqest::ERR;
		}

		try
		{
			size_t count_read = 0;
			_dds::SampleInfo info;

			while (data.size() > 1) data.pop_front();

			while (_datareader->get_unread_count() != 0 && count_read < count_read_packets)
			{
				_datareader->take_next_sample(_data_dds.get(), &info);
				count_read++;
				write_to_deque(_data_dds.get(), count_read);
			}

			*_data = &data;
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error ReadData : error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error ReadData : error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::WriteData(const std::deque<SetTags>& _data)
	{
		ResultReqest result = ResultReqest::OK;

		if (current_status.load() != atech::common::Status::OK)
		{
			result = ResultReqest::IGNOR;
			return result;
		}

		if (config.type_transfer != TypeTransfer::PUBLISHER)
		{
			log->Debug("AdapterDDS id-{}: Error WriteData : not supported", this->config.id_adapter);
			return ResultReqest::ERR;
		}

		try
		{  
			for (const SetTags& tags : _data)
			{
				clear_out_buffer(_data_dds.get());
				create_dds_data(tags);
				update_head(tags.time_source, _data_dds.get());
				_datawriter->write(_data_dds.get());
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error WriteData : error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error WriteData : error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}


	template<typename T> ResultReqest AdapterDDS<T>::write_to_deque(DDSData* buf, size_t& count)
	{
		ResultReqest result = ResultReqest::OK;
		SetTags* box;

		if (count == 1)
		{
			box = &data[0];
		}
		else
		{
			data.push_back({});
			data.back().data_int.resize(size_vector_int_settags);
			data.back().data_float.resize(size_vector_float_settags);
			data.back().data_double.resize(size_vector_double_settags);
			data.back().data_char.resize(size_vector_char_settags);
			data.back().data_str.resize(size_vector_str_settags);
			box = &data.back();
		}

		try
		{
			box->time_source = buf->time_source();
			long long time_packet = buf->time_source();

			std::vector<int>& value_int_source = buf->data_int().value();
			std::vector<char>& quality_int_source = buf->data_int().quality();
			std::vector<float>& value_float_source = buf->data_float().value();
			std::vector<char>& quality_float_source = buf->data_float().quality();
			std::vector<double>& value_double_source = buf->data_double().value();
			std::vector<char>& quality_double_source = buf->data_double().quality();
			std::vector<DataChar>& value_char_source = buf->data_char().value();
			std::vector<char>& quality_char_source = buf->data_char().quality();

			std::vector<ValueT<int>>& value_int_target = box->data_int;
			std::vector<ValueT<float>>& value_float_target = box->data_float;
			std::vector<ValueT<double>>& value_double_target = box->data_double;
			std::vector<ValueT<char>>& value_char_target = box->data_char;
			std::vector<ValueT<std::string>>& value_str_target = box->data_str;

			for (auto& it : config.vec_tags_source)
			{
				if (it.type == TypeValue::INT)
				{
					value_int_target[it.offset_store].value = value_int_source[it.offset];
					value_int_target[it.offset_store].quality = quality_int_source[it.offset];
					value_int_target[it.offset_store].time = time_packet;
					continue;
				}
				if (it.type == TypeValue::FLOAT)
				{
					value_float_target[it.offset_store].value = value_float_source[it.offset];
					value_float_target[it.offset_store].quality = quality_float_source[it.offset];
					value_float_target[it.offset_store].time = time_packet;
					continue;
				}
				if (it.type == TypeValue::DOUBLE)
				{
					value_double_target[it.offset_store].value = value_double_source[it.offset];
					value_double_target[it.offset_store].quality = quality_double_source[it.offset];
					value_double_target[it.offset_store].time = time_packet;
					continue;
				}
				if (it.type == TypeValue::CHAR)
				{
					continue;
				}
				if (it.type == TypeValue::STRING)
				{
					value_str_target[it.offset_store].value = std::string(&value_char_source[it.offset].value()[0], value_char_source[it.offset].value().size());
					value_str_target[it.offset_store].quality = quality_char_source[it.offset];
					value_str_target[it.offset_store].time = time_packet;
				}
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}		

		return  result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::write_to_deque(DDSDataEx* buf, size_t& count)
	{
		ResultReqest result = ResultReqest::OK;
		SetTags* box;
		std::unordered_map<unsigned int, InfoTag>::iterator it;

		if (count != 1)
		{
			data.push_back({});
			data.back() = (*(++data.rbegin()));
		}
		box = &data.back();
		
		try
		{
			box->time_source = buf->time_service();

			{	
				std::vector<DataExInt>& vec = buf->data_int();
				for (DataExInt& source: vec)
				{
					it = map_infotag_to_idtag.find(source.id_tag());
					if (it != map_infotag_to_idtag.end())
					{
						InfoTag& infotag = (*it).second;
						ValueT<int>& target = box->data_int[infotag.offset_store];
						target.value = source.value();
						target.quality = source.quality();
						target.time = target.time;
					}
				}
			}

			{
				std::vector<DataExFloat>& vec = buf->data_float();
				for (DataExFloat& source : vec)
				{
					it = map_infotag_to_idtag.find(source.id_tag());
					if (it != map_infotag_to_idtag.end())
					{
						InfoTag& infotag = (*it).second;
						ValueT<float>& target = box->data_float[infotag.offset_store];
						target.value = source.value();
						target.quality = source.quality();
						target.time = target.time;
					}
				}
			}


			{
				std::vector<DataExDouble>& vec = buf->data_double();
				for (DataExDouble& source : vec)
				{
					it = map_infotag_to_idtag.find(source.id_tag());
					if (it != map_infotag_to_idtag.end())
					{
						InfoTag& infotag = (*it).second;
						ValueT<double>& target = box->data_double[infotag.offset_store];
						target.value = source.value();
						target.quality = source.quality();
						target.time = target.time;
					}
				}
			}

			{
				std::vector<DataExChar>& vec = buf->data_char();
				for (DataExChar& source : vec)
				{
					it = map_infotag_to_idtag.find(source.id_tag());
					if (it != map_infotag_to_idtag.end())
					{
						InfoTag& infotag = map_infotag_to_idtag[source.id_tag()];
						ValueT<std::string>& target = box->data_str[infotag.offset_store];
						target.value = std::string(&source.value()[0], source.value().size());
						target.quality = source.quality();
						target.time = target.time;
					}
				}
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}
		
		
		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::write_to_deque(DDSAlarm* buf, size_t& count)
	{
		ResultReqest result = ResultReqest::OK;
		SetTags* box;

		try
		{
			if (count == 1)
			{
				box = &data[0];
			}
			else
			{
				data.push_back({});
				data.back().data_int.resize(size_vector_int_settags);
				data.back().data_float.resize(size_vector_float_settags);
				data.back().data_double.resize(size_vector_double_settags);
				data.back().data_char.resize(size_vector_char_settags);
				data.back().data_str.resize(size_vector_str_settags);
				box = &data.back();
			}

			box->time_source = buf->time_source();
			long long time_packet = buf->time_source();

			{
				std::vector<unsigned int>& value_source = buf->alarms();
				std::vector<unsigned int>& quality_source = buf->quality();
				std::vector<ValueT<int>>& target = box->data_int;

				for (auto it : config.vec_tags_source)
				{
					target[it.offset_store].value = (int)value_source[it.offset];
					target[it.offset_store].quality = quality_source[it.offset];
					target[it.offset_store].time = time_packet;
				}
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::write_to_deque(DDSAlarmEx* buf, size_t& count)
	{
		ResultReqest result = ResultReqest::OK;
		SetTags* box;
		std::unordered_map<unsigned int, InfoTag>::iterator it;

		try
		{
			if (count != 1)
			{
				data.push_back(data.back());
			}
			box = &data.back();

			box->time_source = buf->time_service();

			{
				std::vector<Alarm>& vec = buf->alarms();
				for (Alarm& source : vec)
				{
					it = map_infotag_to_idtag.find(source.id_tag());
					if (it != map_infotag_to_idtag.end())
					{
						InfoTag& infotag = (*it).second;
						ValueT<char>& target = box->data_char[infotag.offset_store];
						target.value = source.value();
						target.quality = source.quality();
						target.time = source.time_source();
					}
				}
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id_adapter, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id_adapter, 0);
			result = ResultReqest::ERR;

		}

		return result;
	}

	
	template<typename T> ResultReqest AdapterDDS<T>::create_dds_data(const SetTags& in_data)
	{
		for (LinkTags& link : this->config.vec_link_tags)
		{
			if (link.source.type == TypeValue::INT)
			{
				set_data(in_data.data_int[link.source.offset_store], link, _data_dds.get());
				continue;
			}

			if (link.source.type == TypeValue::FLOAT)
			{
				set_data(in_data.data_float[link.source.offset_store], link, _data_dds.get());
				continue;
			}

			if (link.source.type == TypeValue::DOUBLE)
			{
				set_data(in_data.data_double[link.source.offset_store], link, _data_dds.get());
				continue;
			}

			if (link.source.type == TypeValue::CHAR)
			{
				set_data(in_data.data_char[link.source.offset_store], link, _data_dds.get());
				continue;
			}

			if (link.source.type == TypeValue::STRING)
			{
				set_data(in_data.data_str[link.source.offset_store], link, _data_dds.get());
				continue;
			}
		}

		return ResultReqest::OK;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<int>& value, const LinkTags& link, DDSData* out_buf) 
	{
		int val_current = 0;
		int& val_last = out_buf->data_int().value()[link.target.offset];
		char& quality_last = out_buf->data_int().quality()[link.target.offset];

		if (link.target.mask != 0)
		{
			val_last = out_buf->data_int().value()[link.target.offset];
			val_current = demask(value.value, link.source.mask, val_last, link.target.mask);
		}
		else
		{
			val_current = value.value;
		}

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			val_last = val_current;
			quality_last = value.quality;
		}
		else if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == val_current && quality_last == value.quality) 
				return;
			val_last = val_current;
			quality_last = value.quality;
		}
		else if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(val_last - val_current) > (int)link.delta || quality_last != value.quality)
			{
				val_last = val_current;
				quality_last = value.quality;
				return;
			}
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<float>& value, const LinkTags& link, DDSData* out_buf)
	{
		char& quality_last = out_buf->data_float().quality()[link.target.offset];
		float& val_last = out_buf->data_float().value()[link.target.offset];

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			val_last = value.value;
			quality_last = value.quality;
		}
		else if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value && quality_last == value.quality)
				return;
			val_last = value.value;
			quality_last = value.quality;
		}
		else if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(val_last - value.value) > (float)link.delta || quality_last != value.quality)
			{
				val_last = value.value;
				quality_last = value.quality;;
			}
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<double>& value, const LinkTags& link, DDSData* out_buf)
	{
		double& val_last = out_buf->data_double().value()[link.target.offset];
		char& quality_last = out_buf->data_double().quality()[link.target.offset];

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			val_last = value.value;
			quality_last = value.quality;
		}
		else if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value && quality_last == value.quality) 
				return;
			val_last = value.value;
			quality_last = value.quality;
		}
		else if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(val_last - value.value) > link.delta || quality_last != value.quality)
			{
				val_last = value.value;
				quality_last = value.quality;;
				
			}
		}
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<char>& value, const LinkTags& link, DDSData* out_buf)
	{
		ValueT<int> val;
		val.value = value.value;
		val.time = value.time;
		val.quality = value.quality;
		set_data(val, link, out_buf);
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<std::string>& value, const LinkTags& link, DDSData* out_buf)
	{

		std::vector<char>& vchar = out_buf->data_char().value()[link.target.offset].value();

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			copy_str_to_vchar(vchar, value.value);
			out_buf->data_char().quality()[link.target.offset] = value.quality;
		}
		else if (link.type_registration == TypeRegistration::UPDATE || link.type_registration == TypeRegistration::DELTA)
		{			
			if (!is_equal_str_with_vchar(vchar, value.value) || out_buf->data_char().quality()[link.target.offset] != value.quality)
			{
				copy_str_to_vchar(vchar, value.value);
				out_buf->data_char().quality()[link.target.offset] = value.quality;
			}
		}
		return;
	}


	template<typename T> void AdapterDDS<T>::set_data(const ValueT<int>& value, const LinkTags& link, DDSDataEx* out_buf)
	{
		DataExInt dds_value;

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			dds_value.time_source() = value.time;
			dds_value.quality() = value.quality;
			dds_value.value() = value.value;
			dds_value.id_tag() = link.target.id_tag;
			out_buf->data_int().push_back(dds_value);
			return;
		}

		ValueT<int>& _last = _storage.data_int[link.target.offset_store];
		
		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (value.value == _last.value && value.quality == _last.quality) return;
			dds_value.time_source() = value.time;
			dds_value.quality() = value.quality;
			dds_value.value() = value.value;
			dds_value.id_tag() = link.target.id_tag;
			out_buf->data_int().push_back(dds_value);

			_last.value = value.value;
			_last.quality = value.quality;
			_last.time = value.time;
		}
		else if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(value.value - _last.value) > link.delta || value.quality == _last.quality)
			{
				dds_value.time_source() = value.time;
				dds_value.quality() = value.quality;
				dds_value.value() = value.value;
				dds_value.id_tag() = link.target.id_tag;
				out_buf->data_int().push_back(dds_value);

				_last.value = value.value;
				_last.quality = value.quality;
				_last.time = value.time;
			}
		}	
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<float>& value, const LinkTags& link, DDSDataEx* out_buf)
	{
		DataExFloat dds_value;

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			dds_value.time_source() = value.time;
			dds_value.quality() = value.quality;
			dds_value.value() = value.value;
			dds_value.id_tag() = link.target.id_tag;
			out_buf->data_float().push_back(dds_value);
			return;
		}

		ValueT<float>& _last = _storage.data_float[link.target.offset_store];

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (value.value == _last.value && value.quality == _last.quality) return;
			dds_value.time_source() = value.time;
			dds_value.quality() = value.quality;
			dds_value.value() = value.value;
			dds_value.id_tag() = link.target.id_tag;
			out_buf->data_float().push_back(dds_value);

			_last.value = value.value;
			_last.quality = value.quality;
			_last.time = value.time;
		}
		else if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(value.value - _last.value) > link.delta || value.quality == _last.quality)
			{
				dds_value.time_source() = value.time;
				dds_value.quality() = value.quality;
				dds_value.value() = value.value;
				dds_value.id_tag() = link.target.id_tag;
				out_buf->data_float().push_back(dds_value);

				_last.value = value.value;
				_last.quality = value.quality;
				_last.time = value.time;
			}
		}
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<double>& value, const LinkTags& link, DDSDataEx* out_buf)
	{
		DataExDouble dds_value;

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			dds_value.time_source() = value.time;
			dds_value.quality() = value.quality;
			dds_value.value() = value.value;
			dds_value.id_tag() = link.target.id_tag;
			out_buf->data_double().push_back(dds_value);
			return;
		}

		ValueT<double>& _last = _storage.data_double[link.target.offset_store];

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (value.value == _last.value && value.quality == _last.quality) return;
			dds_value.time_source() = value.time;
			dds_value.quality() = value.quality;
			dds_value.value() = value.value;
			dds_value.id_tag() = link.target.id_tag;
			out_buf->data_double().push_back(dds_value);

			_last.value = value.value;
			_last.quality = value.quality;
			_last.time = value.time;
		}
		else if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(value.value - _last.value) > (int)link.delta || value.quality == _last.quality)
			{
				dds_value.time_source() = value.time;
				dds_value.quality() = value.quality;
				dds_value.value() = value.value;
				dds_value.id_tag() = link.target.id_tag;
				out_buf->data_double().push_back(dds_value);

				_last.value = value.value;
				_last.quality = value.quality;
				_last.time = value.time;
			}
		}
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<char>& value, const LinkTags& link, DDSDataEx* out_buf)
	{
		ValueT<int> val;
		val.value = value.value;
		val.time = value.time;
		val.quality = value.quality;

		set_data(val, link, out_buf);
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<std::string>& value, const LinkTags& link, DDSDataEx* out_buf)
	{
		DataExChar dds_value;
		dds_value.value().resize(atech::common::SizeTopics::GetMaxSizeDataExVectorChar());

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			dds_value.time_source() = value.time;
			dds_value.quality() = value.quality;
			copy_str_to_vchar(dds_value.value(), value.value);
			dds_value.id_tag() = link.target.id_tag;
			out_buf->data_char().push_back(dds_value);
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE || link.type_registration == TypeRegistration::DELTA)
		{
			ValueT<std::string>& _last = _storage.data_str[link.target.offset_store];

			if (_last.value.compare(value.value.c_str()) != 0 || _last.quality != value.quality)
			{
				dds_value.time_source() = value.time;
				dds_value.quality() = value.quality;
				dds_value.id_tag() = link.target.id_tag;
				copy_str_to_vchar(dds_value.value(), value.value);
				out_buf->data_char().push_back(dds_value);

				_last.time = value.time;
				_last.quality = value.quality;
				_last.value = value.value;
			}
		}
		
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<int>& value, const LinkTags& link, DDSAlarm* out_buf)
	{
		int val_current = 0;
		int val_last = 0;
		int quality_last = 0;
		int quality_current = 0;

		if (link.target.mask != 0)
		{
			val_last = out_buf->alarms()[link.target.offset];
			quality_last = out_buf->quality()[link.target.offset];
			val_current = demask(value.value, link.source.mask, val_last, link.target.mask);
			quality_current = demask(value.quality, link.source.mask, quality_last, link.target.mask);
		}
		else
		{
			val_current = value.value;
			quality_current = value.quality;
		}

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			out_buf->alarms()[link.target.offset] = val_current;
			out_buf->quality()[link.target.offset] = quality_current;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE || link.type_registration == TypeRegistration::DELTA)
		{
			if (val_last == val_current && quality_last == quality_current) return;
			out_buf->alarms()[link.target.offset] = val_current;
			out_buf->quality()[link.target.offset] = quality_current;
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<float>& value, const LinkTags& link, DDSAlarm* out_buf)
	{
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<double>& value, const LinkTags& link, DDSAlarm* out_buf)
	{
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<char>& value, const LinkTags& link, DDSAlarm* out_buf)
	{
		uint32_t& alarm = out_buf->alarms()[link.target.offset];
		uint32_t& quality = out_buf->quality()[link.target.offset];
		uint32_t value_out = 0;
		uint32_t quality_out = 0;
		if (link.target.mask != 0)
		{
			value_out = demask(value.value, link.source.mask, alarm, link.target.mask);
			quality_out = demask(value.quality, link.source.mask, quality, link.target.mask);
		}
		else
		{
			value_out = (uint32_t)value.value;
			quality_out = value.quality;
		}

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			alarm = value_out;
			quality = quality_out;
		}

		if (link.type_registration == TypeRegistration::UPDATE || link.type_registration == TypeRegistration::DELTA)
		{
			if (alarm == value_out && quality == quality_out) return;
			alarm = value_out;
			quality = quality_out;
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<std::string>& value, const LinkTags& link, DDSAlarm* out_buf)
	{
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<int>& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		int val_current = 0;

		ValueT<int>& _last = _storage.data_int[link.target.id_tag];
		Alarm val_alarm;

		if (link.target.mask != 0)
		{
			val_current = demask(value.value, link.source.mask, _last.value, link.target.mask);
		}
		else
		{
			val_current = value.value;
		}
		
		if (link.type_registration == TypeRegistration::RECIVE)
		{
			val_alarm.value() = (char)val_current;
			val_alarm.quality() = (char)value.quality;
			val_alarm.id_tag() = link.target.id_tag;
			val_alarm.time_source() = value.time;
			out_buf->alarms().push_back(val_alarm);

			_last.value = (char)val_current;
			_last.quality = (char)value.quality;
			_last.time = value.time;
			
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE || link.type_registration == TypeRegistration::DELTA)
		{
			if (_last.value == (char)val_current && _last.quality == value.quality) return;

			val_alarm.value() = (char)val_current;
			val_alarm.quality() = (char)value.quality;
			val_alarm.id_tag() = link.target.id_tag;
			val_alarm.time_source() = value.time;
			out_buf->alarms().push_back(val_alarm);

			_last.value = (char)val_current;
			_last.quality = (char)value.quality;
			_last.time = value.time;
			return;
		}

		return;
	}
	
	template<typename T> void AdapterDDS<T>::set_data(const ValueT<float>& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		return;
	};

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<double>& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		return;
	};

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<char>& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		char val_current = 0;

		ValueT<char>& _last = _storage.data_char[link.target.id_tag];
		Alarm val_alarm;

		if (link.target.mask != 0)
		{
			val_current = demask(value.value, link.source.mask, _last.value, link.target.mask);
		}
		else
		{
			val_current = value.value;
		}

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			val_alarm.value() = val_current;
			val_alarm.quality() = value.quality;
			val_alarm.id_tag() = link.target.id_tag;
			val_alarm.time_source() = value.time;
			out_buf->alarms().push_back(val_alarm);

			_last.value = (char)val_current;
			_last.quality = (char)value.quality;
			_last.time = value.time;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE || link.type_registration == TypeRegistration::DELTA)
		{
			if (_last.value == val_current && _last.quality == value.quality) return;

			val_alarm.value() = (char)val_current;
			val_alarm.quality() = (char)value.quality;
			val_alarm.id_tag() = link.target.id_tag;
			val_alarm.time_source() = value.time;
			out_buf->alarms().push_back(val_alarm);

			_last.value = val_current;
			_last.quality = value.quality;
			_last.time = value.time;
			return;
		}

		return;
	};

	template<typename T> void AdapterDDS<T>::set_data(const ValueT<std::string>& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		return;
	};


	template<typename T> void AdapterDDS<T>::update_head(const long long& time_sourse, T* buf)
	{
		buf->time_service() = TimeConverter::GetTime_LLmcs();
		if constexpr (std::is_same_v<T, DDSData> || std::is_same_v<T, DDSAlarm>)
		{
			buf->time_source() = time_sourse;
		}
		return;
	};

	template<typename T> ResultReqest AdapterDDS<T>::init_buffer(T* buf)
	{
		ResultReqest result = ResultReqest::OK;

		if constexpr (std::is_same_v<T, DDSData>)
		{
			size_t max_int = 0;
			size_t max_float = 0;
			size_t max_double = 0;
			size_t max_str = 0;

			size_t offset_int = 0;
			size_t offset_float = 0;
			size_t offset_double = 0;
			size_t offset_str = 0;


			if (config.type_transfer == TypeTransfer::SUBSCRIBER) return result;

			for (LinkTags& link_tag : this->config.vec_link_tags)
			{
				if (link_tag.target.type == TypeValue::INT)
				{
					if (link_tag.target.offset >= offset_int)
					{
						max_int = link_tag.target.offset + 1;
						offset_int = link_tag.target.offset;
						continue;
					}
				}

				if (link_tag.target.type == TypeValue::FLOAT)
				{
					if (link_tag.target.offset >= offset_float)
					{
						max_float = link_tag.target.offset + 1;
						offset_float = link_tag.target.offset;
						continue;
					}
				}

				if (link_tag.target.type == TypeValue::DOUBLE)
				{
					if (link_tag.target.offset >= offset_double)
					{
						max_double = link_tag.target.offset + 1;
						offset_double = link_tag.target.offset;
						continue;
					}
				}

				if (link_tag.target.type == TypeValue::STRING)
				{
					if (link_tag.target.offset >= offset_str)
					{
						max_str = link_tag.target.offset + 1;
						offset_str = link_tag.target.offset;
						continue;
					}
				}
			}

			buf->data_int().value().resize(max_int);
			buf->data_int().quality().resize(max_int);

			buf->data_float().value().resize(max_float);
			buf->data_float().quality().resize(max_float);

			buf->data_double().value().resize(max_double);
			buf->data_double().quality().resize(max_double);

			buf->data_char().value().resize(max_str);
			buf->data_char().quality().resize(max_str);

			std::vector<DataChar>& vec_char = buf->data_char().value();

			for (DataChar& _char : vec_char)
			{
				_char.value().resize(atech::common::SizeTopics::GetMaxSizeDataChar());
			}
		}

		if constexpr (std::is_same_v<T, DDSAlarm>)
		{
			size_t max_alarms = 0;

			if (config.type_transfer == TypeTransfer::SUBSCRIBER) return result;

			for (LinkTags& link_tag : this->config.vec_link_tags)
			{
				if (link_tag.target.type == TypeValue::INT)
				{
					if (link_tag.target.offset > max_alarms) max_alarms = link_tag.target.offset;
					continue;
				}
			}

			buf->alarms().resize(max_alarms);
			buf->quality().resize(max_alarms);
		}
		else
		{
		}

		return result;
	}

	/*template<typename T> ResultReqest AdapterDDS<T>::init_buffer(DDSData* buf)
	{
		ResultReqest result = ResultReqest::OK;

		size_t max_int = 0;
		size_t max_float = 0;
		size_t max_double = 0;
		size_t max_str = 0;
		
		size_t offset_int = 0;
		size_t offset_float = 0;
		size_t offset_double = 0;
		size_t offset_str = 0;


		if (config.type_transfer == TypeTransfer::SUBSCRIBER) return result;

		for (LinkTags& link_tag : this->config.vec_link_tags)
		{
			if (link_tag.target.type == TypeValue::INT)
			{
				if (link_tag.target.offset >= offset_int)
				{
					max_int = link_tag.target.offset +1;
					offset_int = link_tag.target.offset;
					continue;
				}
			}

			if (link_tag.target.type == TypeValue::FLOAT)
			{
				if (link_tag.target.offset >= offset_float)
				{
					max_float = link_tag.target.offset + 1;
					offset_float = link_tag.target.offset;
					continue;
				}
			}

			if (link_tag.target.type == TypeValue::DOUBLE)
			{
				if (link_tag.target.offset >= offset_double)
				{
					max_double = link_tag.target.offset + 1;
					offset_double = link_tag.target.offset;
					continue;
				}
			}

			if (link_tag.target.type == TypeValue::STRING)
			{
				if (link_tag.target.offset >= offset_str)
				{
					max_str = link_tag.target.offset + 1;
					offset_str = link_tag.target.offset;
					continue;
				}
			}
		}

		buf->data_int().value().resize(max_int);
		buf->data_int().quality().resize(max_int);

		buf->data_float().value().resize(max_float);
		buf->data_float().quality().resize(max_float);

		buf->data_double().value().resize(max_double);
		buf->data_double().quality().resize(max_double);

		buf->data_char().value().resize(max_str);
		buf->data_char().quality().resize(max_str);

		std::vector<DataChar>& vec_char = buf->data_char().value();

		for (DataChar& _char : vec_char)
		{
			_char.value().resize(atech::common::SizeTopics::GetMaxSizeDataChar());
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_buffer(DDSDataEx* buf)
	{
		return ResultReqest::OK;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_buffer(DDSAlarm* buf)
	{
		ResultReqest result = ResultReqest::OK;

		size_t max_alarms = 0;

		if (config.type_transfer == TypeTransfer::SUBSCRIBER) return result;

		for (LinkTags& link_tag : this->config.vec_link_tags)
		{
			if (link_tag.target.type == TypeValue::INT)
			{
				if (link_tag.target.offset > max_alarms) max_alarms = link_tag.target.offset;
				continue;
			}
		}

		buf->alarms().resize(max_alarms);
		buf->quality().resize(max_alarms);

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_buffer(DDSAlarmEx* buf)
	{
		return ResultReqest::OK;
	}*/

	template<typename T> void AdapterDDS<T>::clear_out_buffer(T* buf)
	{
		if constexpr (std::is_same_v<T, DDSDataEx>)
		{
			buf->data_int().resize(0);
			buf->data_float().resize(0);
			buf->data_double().resize(0);
			buf->data_char().resize(0);
		}

		if constexpr (std::is_same_v<T, DDSAlarmEx>)
		{
			buf->alarms().resize(0);
		}

		return;
	};

	/*template<typename T> void AdapterDDS<T>::clear_out_buffer(DDSData* buf)
	{
		return;
	};

	template<typename T> void AdapterDDS<T>::clear_out_buffer(DDSDataEx* buf)
	{
		buf->data_int().resize(0);
		buf->data_float().resize(0);
		buf->data_double().resize(0);
		buf->data_char().resize(0);
		return;
	};

	template<typename T> void AdapterDDS<T>::clear_out_buffer(DDSAlarm* buf)
	{
		return;
	};

	template<typename T> void AdapterDDS<T>::clear_out_buffer(DDSAlarmEx* buf)
	{
		buf->alarms().resize(0);
		return;
	};*/


	template<typename T> int AdapterDDS<T>::demask(const int& value, int mask_source, const int& value_target, const int& mask_target)
	{
		int val_out = value & mask_source;
		if (mask_target > mask_source)
		{
			while (mask_target != mask_source)
			{
				mask_source <<= 1;
				val_out <<= 1;
			}
		}
		else
		{
			while (mask_target != mask_source)
			{
				mask_source >>= 1;
				val_out >>= 1;
			}
		}

		val_out = value_target & (~mask_target) | val_out;
		return val_out;
	}

	template<typename T> void AdapterDDS<T>::copy_str_to_vchar(std::vector<char>& vchar, const std::string& str)
	{
		std::_String_const_iterator str_it = str.begin();
		std::_String_const_iterator str_end = str.end();
		std::vector<char>::iterator vchar_it = vchar.begin();
		std::vector<char>::iterator vchar_end = vchar.end();
		
		for(; str_it != str_end &&  vchar_it != vchar_end; str_it++, vchar_it++)
		{
			*vchar_it = *str_it;
		}

		if (vchar_it != vchar_end) std::fill(vchar_it, vchar_end, '\0');
	}

	template<typename T> bool AdapterDDS<T>::is_equal_str_with_vchar(std::vector<char>& vchar, const std::string& str)
	{
		std::_String_const_iterator str_it = str.begin();
		std::_String_const_iterator str_end = str.begin();
		std::vector<char>::iterator vchar_it = vchar.begin();
		std::vector<char>::iterator vchar_end = vchar.end();

		for (; str_it != str_end && vchar_it != vchar_end; str_it++, vchar_it++)
		{
			if (*str_it != *vchar_it) return false;
		}

		if (vchar_it != vchar_end)
		{
			if (*vchar_it != '\0') return false;
		}

		return true;
	};


	template<typename T> TypeAdapter  AdapterDDS<T>::GetTypeAdapter()
	{
		return TypeAdapter::DDS;

	}

	template<typename T> std::string AdapterDDS<T>::get_name_topic()
	{
		std::string str;
		str += config.topic_name;
		return str;
	}

	template<typename T> std::string AdapterDDS<T>::get_name_topic_profile()
	{
		std::string str;
		str += config.topic_name + "_profile";
		return str;
	}

	template<typename T> std::string AdapterDDS<T>::get_name_participant_profile()
	{
		std::string str;
		str += "participant_profile";
		return str;
	};

	template<typename T> std::string AdapterDDS<T>::get_typetopic(const TypeDDSData& type_ddsdata)
	{
		std::string str;

		switch (type_ddsdata)
		{
		case TypeDDSData::DDSData:
			str += "DDSData";
			break;
		case TypeDDSData::DDSDataEx:
			str += "DDSDataEx";
			break;
		case TypeDDSData::DDSAlarm:
			str += "DDSAlarm";
			break;
		case TypeDDSData::DDSAlarmEx:
			str += "DDSAlarmEx";
			break;
		default:
			str.clear();
		};

		return str;
	}

	template<typename T> std::string AdapterDDS<T>::get_name_publisher_profile()
	{
		std::string str;
		str += "publisher_profile";
		return str;
	}

	template<typename T> std::string AdapterDDS<T>::get_name_subscriber_profile()
	{
		std::string str;
		str += "subscriber_profile";
		return str;
	}

	template<typename T> std::string  AdapterDDS<T>::get_name_datawriter_profile()
	{
		std::string str;
		str += this->config.topic_name+"datawrite_profile";
		return str;
	}

	template<typename T> std::string  AdapterDDS<T>::get_name_datareader_profile()
	{
		std::string str;
		str += this->config.topic_name + "dataread_profile";
		return str;
	}

	template<typename T> std::shared_ptr<IAnswer>  AdapterDDS<T>::GetInfo(ParamInfoAdapter param)
	{
		return nullptr;
	}

	template<typename T> uint32_t AdapterDDS<T>::GetId()
	{
		return config.id_adapter;
	}

	template<typename T> ResultReqest AdapterDDS<T>::GetStatus(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		st.push_back({ this->config.id_adapter, current_status.load() });
		return ResultReqest::OK;
	}

	template<typename T> ResultReqest AdapterDDS<T>::Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };

		if (current_status.load() == atech::common::Status::STOP ||
			current_status.load() == atech::common::Status::OK)
		{
			current_status.store(atech::common::Status::OK);
			result = ResultReqest::OK;
		}
		else
		{
			result = ResultReqest::ERR;
		}

		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };

		if (current_status.load() == atech::common::Status::STOP ||
			current_status.load() == atech::common::Status::OK)
		{
			current_status.store(atech::common::Status::STOP);
			result = ResultReqest::OK;
		}
		else
		{
			result = ResultReqest::ERR;
		}

		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		this->destroy();
		result = this->init_adapter();
		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

	template<typename T> void AdapterDDS<T>::destroy()
	{
		log->Debug("AdapterDDS id-{}: Start delete", this->config.id_adapter);
		
		if (_datawriter != nullptr)
		{
			_factory_dds->delete_datawriter(_datawriter);
		}

		if (_datareader != nullptr)
		{
			_factory_dds->delete_datareader(_datareader);
		}

		_factory_dds->unregistration_topic(config.topic_name);

		log->Debug("AdapterDDS id-{}: Delete done", this->config.id_adapter);

		current_status.store(atech::common::Status::NONE);

		return;
	}

	template<typename T> atech::srv::io::TypeTopic AdapterDDS<T>::typetopic_to_typeDataDDS(const TypeDDSData & type_dds)
	{
		if (type_dds == TypeDDSData::DDSData) return  atech::srv::io::TypeTopic::DDSData;
		if (type_dds == TypeDDSData::DDSDataEx) return  atech::srv::io::TypeTopic::DDSDataEx;
		if (type_dds == TypeDDSData::DDSAlarm) return  atech::srv::io::TypeTopic::DDSAlarm;
		if (type_dds == TypeDDSData::DDSAlarmEx) return  atech::srv::io::TypeTopic::DDSAlarmEx;
		return atech::srv::io::TypeTopic::DDSData;
	}

} 
