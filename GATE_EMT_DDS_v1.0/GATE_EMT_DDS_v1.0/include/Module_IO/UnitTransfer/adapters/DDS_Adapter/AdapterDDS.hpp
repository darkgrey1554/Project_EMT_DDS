#pragma once
#include <LoggerScada.hpp>
#include <Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include <structs/TimeConverter.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>


#include <TypeTopicDDS/TypeTopics.h>


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

		unsigned int id = 0;
		std::string topic_name = "";
		TypeTransfer type_transfer = TypeTransfer::SUBSCRIBER;
		TypeDDSData type_data;
		std::string str_config_ddslayer;
	};

	struct StoreLastValue
	{
		std::unordered_map<int, ValueInt> map_int;
		std::unordered_map<int, ValueFloat> map_float;
		std::unordered_map<int, ValueString> map_str;
		std::unordered_map<int, ValueDouble> map_double;
		std::unordered_map<int, ValueChar> map_char;
	};

	template<typename T>
	class AdapterDDS : public IAdapter
	{
		
		ConfigAdapterDDS config;
		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null;
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// כמדדונ

		_dds::DomainParticipant* _participant = nullptr;
		_dds::Subscriber* _subscriber = nullptr;
		_dds::DataReader* _datareader = nullptr;
		_dds::Publisher* _publisher = nullptr;
		_dds::DataWriter* _datawriter = nullptr;
		_dds::Topic* _topic_data = nullptr;
		std::shared_ptr<T> _data_dds;

		//std::vector<SetTags> _data;
		SetTags template_settags;
		std::map<unsigned int, InfoTag> map_infotag_to_idtag;
		StoreLastValue _storage;

		size_t count_read_packets = 10;

		ResultReqest init_participant();
		ResultReqest registration_type();
		ResultReqest init_topic();
		ResultReqest init_transport();
		ResultReqest init_publisher();
		ResultReqest init_subscriber();
		ResultReqest init_template_settags();
		ResultReqest init_map_infotags_to_idtag();
		ResultReqest init_buffer(DDSData* buf);
		ResultReqest init_buffer(DDSDataEx* buf);
		ResultReqest init_buffer(DDSAlarm* buf);
		ResultReqest init_buffer(DDSAlarmEx* buf);
		ResultReqest init_StoreLastValue();
		
		std::string get_name_participant_profile();
		std::string get_name_topic();
		std::string get_name_topic_profile();
		std::string get_typetopic(const TypeDDSData& type_ddsdata);
		std::string get_name_publisher_profile();
		std::string get_name_subscriber_profile();
		std::string get_name_datawriter_profile();

		ResultReqest write_to_deque(DDSData* buf, size_t& count);
		ResultReqest write_to_deque(DDSDataEx* buf, size_t& count);
		ResultReqest write_to_deque(DDSAlarm* buf, size_t& count);
		ResultReqest write_to_deque(DDSAlarmEx* buf, size_t& count);

		ResultReqest create_dds_data(const SetTags& in_data);

		void clear_out_buffer(DDSData* buf);
		void clear_out_buffer(DDSDataEx* buf);
		void clear_out_buffer(DDSAlarm* buf);
		void clear_out_buffer(DDSAlarmEx* buf);
	
		void update_head(const long long& time_sourse, DDSData* buf);
		void update_head(const long long& time_sourse, DDSDataEx* buf);
		void update_head(const long long& time_sourse, DDSAlarm* buf);
		void update_head(const long long& time_sourse, DDSAlarmEx* buf);

		void set_data(const ValueInt& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueInt& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueInt& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueInt& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void set_data(const ValueFloat& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueFloat& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueFloat& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueFloat& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void set_data(const ValueDouble& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueDouble& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueDouble& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueDouble& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void set_data(const ValueChar& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueChar& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueChar& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueChar& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void set_data(const ValueString& value, const LinkTags& link, DDSData* out_buf);
		void set_data(const ValueString& value, const LinkTags& link, DDSDataEx* out_buf);
		void set_data(const ValueString& value, const LinkTags& link, DDSAlarm* out_buf);
		void set_data(const ValueString& value, const LinkTags& link, DDSAlarmEx* out_buf);

		void copy_str_to_vchar(std::vector<char>& vchar, const std::string& str);
		bool is_equal_str_with_vchar(std::vector<char>& vchar, const std::string& str);
		int demask(const int& value, int mask_source, const int& value_target, const int& mask_target);

	public:

		ResultReqest InitAdapter() override;
		ResultReqest ReadData(std::deque<SetTags>** _data) override;
		ResultReqest WriteData(const std::deque<SetTags>& _data) override;
		TypeAdapter GetTypeAdapter() override;
		StatusAdapter GetStatusAdapter() override;
		std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param) override;

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

		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	};

	template<class T> AdapterDDS<T>::~AdapterDDS()
	{
		if (_datawriter != nullptr)
		{
			_publisher->delete_datawriter(_datawriter);
		}

		if (_datareader != nullptr)
		{
			_subscriber->delete_datareader(_datareader);
		}

		if (_publisher != nullptr)
		{
			_participant->delete_publisher(_publisher);
		}

		if (_subscriber != nullptr)
		{
			_participant->delete_subscriber(_subscriber);
		}

		if (_topic_data != nullptr)
		{
			_participant->delete_topic(_topic_data);
		}

		if (_participant != nullptr)
		{
			eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(_participant);
		}
	};

	template<typename T> ResultReqest  AdapterDDS<T>::InitAdapter()
	{
		ResultReqest result = ResultReqest::OK;

		/// --- guard from repeated usage --- ///
		const std::lock_guard<std::mutex> lock_init(mutex_init);

		StatusAdapter status = current_status.load(std::memory_order::memory_order_relaxed);
		if (status == StatusAdapter::INITIALIZATION || status == StatusAdapter::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}
		current_status.store(StatusAdapter::INITIALIZATION, std::memory_order_relaxed);

		try
		{
			if (config.type_adapter != TypeAdapter::DDS) throw 1;

			if (init_participant() != ResultReqest::OK) throw 2;

			if (registration_type() != ResultReqest::OK);

			if (init_topic() != ResultReqest::OK) throw 3;

			if (init_transport() != ResultReqest::OK) throw 4;

			if (init_template_settags() != ResultReqest::OK) throw 5;

			_data_dds = std::make_shared<T>(T());

			if (init_buffer(_data_dds.get()) != ResultReqest::OK) throw 6;

			if (init_StoreLastValue() != ResultReqest::OK) throw;

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error Init : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Init : error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_participant()
	{

		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!config.str_config_ddslayer.empty())
			{
				_dds::DomainParticipantFactory::get_instance()->load_XML_profiles_string(config.str_config_ddslayer.c_str(), config.str_config_ddslayer.size());
			}

			_participant = _dds::DomainParticipantFactory::get_instance()->create_participant_with_profile(get_name_participant_profile());
			if (!_participant)
			{
				_participant = _dds::DomainParticipantFactory::get_instance()->create_participant(0, _dds::PARTICIPANT_QOS_DEFAULT);
				if (!_participant)
				{
					throw 1;
				}

				log->Debug("AdapterDDS id-{}: Init participant done : defualt", this->config.id);
			}
			else
			{
				log->Debug("AdapterDDS id-{}: Init participant done : XML-file", this->config.id);
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error init participant : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error init participant : error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::registration_type()
	{

		ResultReqest result = ResultReqest::OK;

		try
		{
			_dds::TypeSupport PtrSupporTypeDDSData(new DDSDataPubSubType());
			if (PtrSupporTypeDDSData.register_type(_participant) != ReturnCode_t::RETCODE_OK) throw 1;
		}
		catch (...)
		{
			log->Debug("AdapterDDS id - {}: Error registration DDSData", config.id);
			result = ResultReqest::ERR;
		}

		try
		{
			_dds::TypeSupport PtrSupporTypeDDSDataEx(new DDSDataExPubSubType());
			if (PtrSupporTypeDDSDataEx.register_type(_participant) != ReturnCode_t::RETCODE_OK) throw 1;
		}
		catch (...)
		{
			log->Debug("AdapterDDS id - {}: Error registration DDSDataEx", config.id);
			result = ResultReqest::ERR;
		}

		try
		{
			_dds::TypeSupport PtrSupporTypeDDSAlarm(new DDSAlarmPubSubType());
			if (PtrSupporTypeDDSAlarm.register_type(_participant) != ReturnCode_t::RETCODE_OK) throw 1;
		}
		catch (...)
		{
			log->Debug("AdapterDDS id - {}: Error registration DDSAlarm", config.id);
			result = ResultReqest::ERR;
		}

		try
		{
			_dds::TypeSupport PtrSupporTypeDDSAlarmEx(new DDSAlarmExPubSubType());
			if (PtrSupporTypeDDSAlarmEx.register_type(_participant) != ReturnCode_t::RETCODE_OK) throw 1;
		}
		catch (...)
		{
			log->Debug("AdapterDDS id - {}: Error registration DDSAlarmEx", config.id);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_topic()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!_participant)
			{
				throw 1;
			}

			_topic_data = _participant->create_topic_with_profile(get_name_topic(),
				get_typetopic(config.type_data),
				get_name_topic_profile());

			if (!_topic_data)
			{
				_topic_data = _participant->create_topic(get_name_topic(),
					get_typetopic(config.type_data),
					_dds::TOPIC_QOS_DEFAULT);
				if (!_topic_data)
				{
					throw 1;
				}

				log->Debug("AdapterDDS id-{}: Init topic done : defualt", this->config.id);
			}
			else
			{
				log->Debug("AdapterDDS id-{}: Init topic done : XML-file", this->config.id);
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error init topic : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error init tipic : error: {}", this->config.id, 0);
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
			if (!_participant) throw 1;

			_publisher = _participant->create_publisher_with_profile(get_name_publisher_profile());

			if (!_publisher)
			{
				_publisher = _participant->create_publisher(_dds::PUBLISHER_QOS_DEFAULT);
				if (!_publisher) throw 2;

				log->Debug("AdapterDDS id-{}: Init publisher done : default", this->config.id);
			}
			else
			{
				log->Debug("AdapterDDS id-{}: Init publisher done : XML-file", this->config.id);
			}

			_datawriter = _publisher->create_datawriter_with_profile(_topic_data, get_name_datawriter_profile());

			if (!_datawriter)
			{
				_datawriter = _publisher->create_datawriter(_topic_data, _dds::DATAWRITER_QOS_DEFAULT);
				if (!_datawriter)
				{
					throw 3;
				}
				else
				{
					log->Debug("AdapterDDS id-{}: Init datawriter done : default", this->config.id);
				}
			}
			else
			{
				log->Debug("AdapterDDS id-{}: Init datawriter done : XML-file", this->config.id);
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Init publisher : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Init publisher: error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_subscriber()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!_participant) throw 1;

			_subscriber = _participant->create_subscriber_with_profile(get_name_subscriber_profile());

			if (!_subscriber)
			{
				_subscriber = _participant->create_subscriber(_dds::SUBSCRIBER_QOS_DEFAULT);
				if (!_subscriber) throw 2;

				log->Debug("AdapterDDS id-{}: Init subscriber done : default", this->config.id);
			}
			else
			{
				log->Debug("AdapterDDS id-{}: Init subscriber done : XML-file", this->config.id);
			}

			_datareader = _subscriber->create_datareader_with_profile(_topic_data, get_name_datawriter_profile());

			if (!_datareader)
			{
				_datareader = _subscriber->create_datareader(_topic_data, _dds::DATAREADER_QOS_DEFAULT);
				if (!_datareader)
				{
					if (!_datareader) throw 3;
					log->Debug("AdapterDDS id-{}: Init datareader done : default", this->config.id);
				}
				else
				{
					log->Debug("AdapterDDS id-{}: Init datareader done : XML-file", this->config.id);
				}
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Init subscriber : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Init subscriber : error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_template_settags()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			for (InfoTag& tag : this->config.vec_tags_source)
			{
				if (tag.type == TypeValue::INT)
				{
					template_settags.map_int_data[tag] = { 0,0,0 };
					continue;
				}

				if (tag.type == TypeValue::FLOAT)
				{
					template_settags.map_float_data[tag] = { 0,0,0 };
					continue;
				}

				if (tag.type == TypeValue::DOUBLE)
				{
					template_settags.map_double_data[tag] = { 0,0,0 };
					continue;
				}
				if (tag.type == TypeValue::CHAR)
				{
					template_settags.map_char_data[tag] = { 0,'\0',0 };
					continue;
				}
				if (tag.type == TypeValue::STRING)
				{
					template_settags.map_str_data[tag] = { 0,"",0 };
					continue;
				}
			}

			data.resize(0);
			data.push_back(template_settags);

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error init tempalte settags : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Error init tempalte settags : error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}
		

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::init_map_infotags_to_idtag()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			for (InfoTag& tag : this->config.vec_link_tasg)
			{
				map_infotag_to_idtag[tag.id_tag] = tag;
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Error Init map of infotag to idtag : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error Init : error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}


	}

	template<typename T> ResultReqest AdapterDDS<T>::init_StoreLastValue()
	{
		ResultReqest result = ResultReqest::OK;

		if (config.type_data == TypeDDSData::DDSData || config.type_data == TypeDDSData::DDSAlarm) return result;

		if (config.type_data == TypeDDSData::DDSDataEx)
		{
			for (LinkTags& link : this->config.vec_link_tags)
			{
				if (link.target.type == TypeValue::INT)
				{
					_storage.map_int[link.target.id_tag] = { 0,0,0 };
					continue;
				}

				if (link.target.type == TypeValue::FLOAT)
				{
					_storage.map_float[link.target.id_tag] = { 0,0,0 };
					continue;
				}

				if (link.target.type == TypeValue::DOUBLE)
				{
					_storage.map_double[link.target.id_tag] = { 0,0,0 };
					continue;
				}

				if (link.target.type == TypeValue::STRING)
				{
					_storage.map_str[link.target.id_tag] = { 0,"",0};
					continue;
				}
			}
		}

		if (config.type_data == TypeDDSData::DDSAlarmEx)
		{
			for (LinkTags& link : this->config.vec_link_tags)
			{
				_storage.map_str[link.target.id_tag] = { 0,"",0 };
			}
		}

	}

	template<typename T> ResultReqest AdapterDDS<T>::ReadData(std::deque<SetTags>** _data)
	{
		ResultReqest result = ResultReqest::OK;

		if (config.type_transfer != TypeTransfer::SUBSCRIBER)
		{
			log->Debug("AdapterDDS id-{}: Error ReadData : not supported", this->config.id);
			return ResultReqest::ERR;
		}

		try
		{
			size_t count_read = 0;
			_dds::SampleInfo info;

			if (data.size() > 1) data.resize(1);

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
			log->Critical("AdapterDDS id-{}: Error ReadData : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error ReadData : error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::WriteData(const std::deque<SetTags>& _data)
	{
		ResultReqest result = ResultReqest::OK;

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
			log->Critical("AdapterDDS id-{}: Error WriteData : error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Error WriteData : error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}



	template<typename T> ResultReqest AdapterDDS<T>::write_to_deque(DDSData* buf, size_t& count)
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			template_settags.time_source = buf->time_source();
			long long time_packet = buf->time_source();

			{
				std::vector<int>& value = buf->data_int().value();
				std::vector<char>& quality = buf->data_int().quality();

				for (auto it = template_settags.map_int_data.begin(); it != template_settags.map_int_data.end(); it++)
				{
					it->second.value = value[it->first.offset];
					it->second.quality = quality[it->first.offset];
					it->second.time = time_packet;
				}
			}

			{
				std::vector<float>& value = buf->data_float().value();
				std::vector<char>& quality = buf->data_float().quality();

				for (auto it = template_settags.map_float_data.begin(); it != template_settags.map_float_data.end(); it++)
				{
					it->second.value = value[it->first.offset];
					it->second.quality = quality[it->first.offset];
					it->second.time = time_packet;
				}
			}

			{
				std::vector<double>& value = buf->data_double().value();
				std::vector<char>& quality = buf->data_double().quality();

				for (auto it = template_settags.map_double_data.begin(); it != template_settags.map_double_data.end(); it++)
				{
					it->second.value = value[it->first.offset];
					it->second.quality = quality[it->first.offset];
					it->second.time = time_packet;
				};
			}

			{
				std::vector<DataChar>& value = buf->data_char().value();
				std::vector<char>& quality = buf->data_char().quality();

				for (auto it = template_settags.map_str_data.begin(); it != template_settags.map_str_data.end(); it++)
				{
					it->second.value.clear();
					it->second.value = std::string(&value[it->first.offset].value()[0], value[it->first.offset].value().size());
					it->second.quality = quality[it->first.offset];
					it->second.time = time_packet;
				}
			}

			if (count == 1)
			{
				data[0] = template_settags; /// need escape copy !!!
			}
			else
			{
				data.push_back(template_settags);
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}		

		return  result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::write_to_deque(DDSDataEx* buf, size_t& count)
	{
		ResultReqest result = ResultReqest::OK;
		
		try
		{
			template_settags.time_source = buf->time_service();

			{
				std::vector<DataExInt>& vec = buf->data_int();
				for (DataExInt& source: vec)
				{
					if (map_infotag_to_idtag.count(source.id_tag()))
					{
						InfoTag& infotag = map_infotag_to_idtag[source.id_tag()];
						ValueInt& target = template_settags.map_int_data[infotag];
						target.value = source.value();
						target.quality = source.quality();
						target.time = target.time;
					}
				}
			}

			{
				std::vector<DataExInt>& vec = buf->data_int();
				for (DataExInt& source : vec)
				{
					if (map_infotag_to_idtag.count(source.id_tag()))
					{
						InfoTag& infotag = map_infotag_to_idtag[source.id_tag()];
						ValueFloat& target = template_settags.map_float_data[infotag];
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
					if (map_infotag_to_idtag.count(source.id_tag()))
					{
						InfoTag& infotag = map_infotag_to_idtag[source.id_tag()];
						ValueDouble& target = template_settags.map_double_data[infotag];
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
					if (map_infotag_to_idtag.count(source.id_tag()))
					{
						InfoTag& infotag = map_infotag_to_idtag[source.id_tag()];
						ValueString& target = template_settags.map_str_data[infotag];
						target.value = std::string(&source.value()[0], source.value().size());
						target.quality = source.quality();
						target.time = target.time;
					}
				}
			}

			if (count == 1)
			{
				data[0] = template_settags; // need escape copy
			}
			else
			{
				data.push_back(template_settags);
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}
		
		
		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::write_to_deque(DDSAlarm* buf, size_t& count)
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			template_settags.time_source = buf->time_source();
			long long time_packet = buf->time_source();

			{
				std::vector<unsigned int>& value = buf->alarms();
				std::vector<unsigned int>& quality = buf->quality();

				for (auto it = template_settags.map_int_data.begin(); it != template_settags.map_int_data.end(); it++)
				{
					it->second.value = value[it->first.offset];
					it->second.quality = quality[it->first.offset];
					it->second.time = time_packet;
				}
			}

			if (count == 1)
			{
				data[0] = template_settags; // need escape copy
			}
			else
			{
				data.push_back(template_settags);
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T> ResultReqest AdapterDDS<T>::write_to_deque(DDSAlarmEx* buf, size_t& count)
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			template_settags.time_source = buf->time_service();

			{
				std::vector<Alarm>& vec = buf->alarms();
				for (Alarm& source : vec)
				{
					if (map_infotag_to_idtag.count(source.id_tag()))
					{
						InfoTag& infotag = map_infotag_to_idtag[source.id_tag()];
						ValueChar& target = template_settags.map_char_data[infotag];
						target.value = source.value();
						target.quality = source.quality();
						target.time = target.time;
					}
				}
			}

			if (count == 1)
			{
				data[0] = template_settags; // need escape copy
			}
			else
			{
				data.push_back(template_settags);
			}

		}
		catch (int& e)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterDDS id-{}: Write_to_vector_tags: error: {}", this->config.id, 0);
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
				auto value = in_data.map_int_data.find(link.source);
				if (value != in_data.map_int_data.end())
				{
					set_data(value->second, link, _data_dds.get());
				}
				continue;
			}

			if (link.source.type == TypeValue::FLOAT)
			{
				auto value = in_data.map_float_data.find(link.source);
				if (value != in_data.map_float_data.end())
				{
					set_data(value->second, link, _data_dds.get());
				}
				continue;
			}

			if (link.source.type == TypeValue::DOUBLE)
			{
				auto value = in_data.map_double_data.find(link.source);
				if (value != in_data.map_double_data.end())
				{
					set_data(value->second, link, _data_dds.get());
				}
				continue;
			}

			if (link.source.type == TypeValue::STRING)
			{
				auto value = in_data.map_str_data.find(link.source);
				if (value != in_data.map_str_data.end())
				{
					set_data(value->second, link, _data_dds.get());
				}
				continue;
			}
		}

		return ResultReqest::OK;
	}


	template<typename T> void AdapterDDS<T>::set_data(const ValueInt& value, const LinkTags& link, DDSData* out_buf) 
	{
		int val_current = 0;
		int val_last = 0;

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
			out_buf->data_int().value()[link.target.offset] = val_current;
			out_buf->data_int().quality()[link.target.offset] = value.quality;
			return;
		}

		char& quality_last = out_buf->data_int().quality()[link.target.offset];

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == val_current && quality_last == value.quality) return;
			out_buf->data_int().value()[link.target.offset] = val_current;
			out_buf->data_int().quality()[link.target.offset] = value.quality;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(val_last - val_current) > (int)link.delta || quality_last != value.quality)
			{
				out_buf->data_int().value()[link.target.offset] = val_current;
				out_buf->data_int().quality()[link.target.offset] = value.quality;;
				return;
			}
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueFloat& value, const LinkTags& link, DDSData* out_buf)
	{
		char& quality_last = out_buf->data_float().quality()[link.target.offset];
		float& val_last = out_buf->data_float().value()[link.target.offset];

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			val_last = value.value;
			quality_last = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value && quality_last == value.quality) return;
			val_last = value.value;
			quality_last = value.quality;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(val_last - value.value) > (float)link.delta || quality_last != value.value)
			{
				val_last = value.value;
				quality_last = value.quality;;
				return;
			}
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueDouble& value, const LinkTags& link, DDSData* out_buf)
	{
		double& val_last = out_buf->data_double().value()[link.target.offset];
		char& quality_last = out_buf->data_double().quality()[link.target.offset];

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			val_last = value.value;
			quality_last = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value && quality_last == value.quality) return;
			val_last = value.value;
			quality_last = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(val_last - value.value) > link.delta || quality_last != value.value)
			{
				val_last = value.value;
				quality_last = value.quality;;
				
			}
			return;
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueChar& value, const LinkTags& link, DDSData* out_buf)
	{
		ValueInt val;
		val.value = (int)value.value;
		val.time = value.time;
		val.quality = value.quality;
		
		set_data(val, link, out_buf);

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueString& value, const LinkTags& link, DDSData* out_buf)
	{

		std::vector<char>& vchar = out_buf->data_char().value()[link.target.offset].value();

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			copy_str_to_vchar(vchar, value.value);
			out_buf->data_char().quality()[link.target.offset] = value.quality;
		}

		if (link.type_registration == TypeRegistration::UPDATE || link.type_registration == TypeRegistration::DELTA)
		{			
			if (!is_equal_str_with_vchar(vchar, value.value) || out_buf->data_char().quality()[link.target.offset] != value.quality)
			{
				copy_str_to_vchar(vchar, value.value);
				out_buf->data_char().quality()[link.target.offset] = value.quality;
			}
		}

		return;
	}


	template<typename T> void AdapterDDS<T>::set_data(const ValueInt& value, const LinkTags& link, DDSDataEx* out_buf)
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

		ValueInt& _last = _storage.map_int[link.target.id_tag];
		
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

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(value.value - _last.value) > (int)link.delta || value.quality == _last.quality)
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

	template<typename T> void AdapterDDS<T>::set_data(const ValueFloat& value, const LinkTags& link, DDSDataEx* out_buf)
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

		ValueFloat& _last = _storage.map_float[link.target.id_tag];

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
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(value.value - _last.value) > (int)link.delta || value.quality == _last.quality)
			{
				dds_value.time_source() = value.time;
				dds_value.quality() = value.quality;
				dds_value.value() = value.value;
				dds_value.id_tag() = link.target.id_tag;
				out_buf->data_float().push_back(dds_value);

				_last.value = value.value;
				_last.quality = value.quality;
				_last.time = value.time;
				return;
			}
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueDouble& value, const LinkTags& link, DDSDataEx* out_buf)
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

		ValueDouble& _last = _storage.map_double[link.target.id_tag];

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
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
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
				return;
			}
		}

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueChar& value, const LinkTags& link, DDSDataEx* out_buf)
	{
		ValueInt val;
		val.value = (int)value.value;
		val.time = value.time;
		val.quality = value.quality;

		set_data(val, link, out_buf);

		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueString& value, const LinkTags& link, DDSDataEx* out_buf)
	{
		DataExChar dds_value;
		dds_value.value().resize(scada_ate::typetopics::GetMaxSizeDataExVectorChar());

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
			ValueString& _last = _storage.map_str[link.target.id_tag];

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

				return;
			}
		}
		
		return;
	}


	template<typename T> void AdapterDDS<T>::set_data(const ValueInt& value, const LinkTags& link, DDSAlarm* out_buf)
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

	template<typename T> void AdapterDDS<T>::set_data(const ValueFloat& value, const LinkTags& link, DDSAlarm* out_buf)
	{
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueDouble& value, const LinkTags& link, DDSAlarm* out_buf)
	{
		return;
	}

	template<typename T> void AdapterDDS<T>::set_data(const ValueChar& value, const LinkTags& link, DDSAlarm* out_buf)
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
			value_out = value.value;
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

	template<typename T> void AdapterDDS<T>::set_data(const ValueString& value, const LinkTags& link, DDSAlarm* out_buf)
	{
		return;
	}


	template<typename T> void AdapterDDS<T>::set_data(const ValueInt& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		int val_current = 0;

		ValueChar& _last = _storage.map_char[link.target.id_tag];
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
	
	template<typename T> void AdapterDDS<T>::set_data(const ValueFloat& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		return;
	};

	template<typename T> void AdapterDDS<T>::set_data(const ValueDouble& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		return;
	};

	template<typename T> void AdapterDDS<T>::set_data(const ValueChar& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		char val_current = 0;

		ValueChar& _last = _storage.map_char[link.target.id_tag];
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

	template<typename T> void AdapterDDS<T>::set_data(const ValueString& value, const LinkTags& link, DDSAlarmEx* out_buf)
	{
		return;
	};


	template<typename T> void AdapterDDS<T>::update_head(const long long& time_sourse, DDSData* buf)
	{
		buf->time_source() = time_sourse;
		buf->time_service() = TimeConverter::GetTime_LLmcs();
		return;
	};

	template<typename T> void AdapterDDS<T>::update_head(const long long& time_sourse, DDSDataEx* buf)
	{
		buf->time_service() = TimeConverter::GetTime_LLmcs();
		return;
	};

	template<typename T> void AdapterDDS<T>::update_head(const long long& time_sourse, DDSAlarm* buf)
	{
		buf->time_source() = time_sourse;
		buf->time_service() = TimeConverter::GetTime_LLmcs();
		return;
	};

	template<typename T> void AdapterDDS<T>::update_head(const long long& time_sourse, DDSAlarmEx* buf)
	{
		buf->time_service() = TimeConverter::GetTime_LLmcs();
		return;
	};


	template<typename T> ResultReqest AdapterDDS<T>::init_buffer(DDSData* buf)
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
			_char.value().resize(scada_ate::typetopics::GetMaxSizeDataChar());
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
	}

	template<typename T> void AdapterDDS<T>::clear_out_buffer(DDSData* buf)
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
	};


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

	template<typename T> StatusAdapter  AdapterDDS<T>::GetStatusAdapter()
	{
		return StatusAdapter::Null;
	}

	template<typename T> std::shared_ptr<IAnswer>  AdapterDDS<T>::GetInfoAdapter(ParamInfoAdapter param)
	{
		return nullptr;
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
		str += "datawrite_profile";
		return str;
	}

} 
