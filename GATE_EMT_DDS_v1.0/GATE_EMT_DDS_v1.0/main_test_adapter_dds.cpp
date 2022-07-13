#include <Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include <Module_IO/UnitTransfer/adapters/DDS_Adapter/AdapterDDS.hpp>
#include <deque>

int main()
{
	int size_data = 10000;
	int count_iter = 1000;
	scada_ate::typetopics::SetMaxSizeDataCollectionInt(size_data);

	std::vector<long long> time_command(count_iter);
	long long _time;

	std::shared_ptr<LoggerSpaceScada::ILoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);

	log->SetLevel(LoggerSpaceScada::LevelLog::Debug);
	log->Info("Start {} {}", 1, 2);


	std::vector <scada_ate::gate::adapter::LinkTags> link_tags(size_data);
	scada_ate::gate::adapter::SetTags set_tags;
	std::vector<scada_ate::gate::adapter::InfoTag> vector_source(size_data);

	int iter = 0;
	for (auto& link : link_tags)
	{
		link.source.id_tag = iter;
		link.source.is_array = 0;
		link.source.mask = 0;
		link.source.offset = 0;
		link.source.tag = "";
		link.source.type = scada_ate::gate::adapter::TypeValue::INT;

		link.target.id_tag = iter;
		link.target.is_array = 0;
		link.target.mask = 0;
		link.target.offset = iter;
		link.target.tag = "";
		link.target.type = scada_ate::gate::adapter::TypeValue::INT;

		link.type_registration = scada_ate::gate::adapter::TypeRegistration::RECIVE;

		vector_source[iter] = link.target;

		set_tags.map_int_data[link.source].value = iter;
		iter++;
	}

	std::deque<scada_ate::gate::adapter::SetTags> dq_settags;
	dq_settags.push_back(set_tags);

	scada_ate::gate::adapter::IAdapter_ptr adapter_dds;
	std::shared_ptr<scada_ate::gate::adapter::dds::ConfigAdapterDDS> config = std::make_shared<scada_ate::gate::adapter::dds::ConfigAdapterDDS>();
	config->id = 1;
	config->str_config_ddslayer = "";
	config->topic_name = "test_topic";
	config->type_adapter = scada_ate::gate::adapter::TypeAdapter::DDS;
	config->type_data = scada_ate::gate::adapter::dds::TypeDDSData::DDSData;
	config->type_transfer = scada_ate::gate::adapter::dds::TypeTransfer::PUBLISHER;
	config->vec_link_tags = link_tags;

	scada_ate::gate::adapter::IAdapter_ptr adapter_dds_sub;
	std::shared_ptr<scada_ate::gate::adapter::dds::ConfigAdapterDDS> config_sub = std::make_shared<scada_ate::gate::adapter::dds::ConfigAdapterDDS>();
	config_sub->id = 2;
	config_sub->str_config_ddslayer = "";
	config_sub->topic_name = "test_topic";
	config_sub->type_adapter = scada_ate::gate::adapter::TypeAdapter::DDS;
	config_sub->type_data = scada_ate::gate::adapter::dds::TypeDDSData::DDSData;
	config_sub->type_transfer = scada_ate::gate::adapter::dds::TypeTransfer::SUBSCRIBER;
	config_sub->vec_tags_source = vector_source;




	adapter_dds = scada_ate::gate::adapter::CreateAdapter(config);
	adapter_dds->InitAdapter();

	adapter_dds_sub = scada_ate::gate::adapter::CreateAdapter(config_sub);
	adapter_dds_sub->InitAdapter();

	adapter_dds->WriteData(dq_settags);

	/*for (int i = 0; i < count_iter; i++)
	{
		_time = TimeConverter::GetTime_LLmcs();
		adapter_dds->WriteData(dq_settags);
		time_command[i] = TimeConverter::GetTime_LLmcs() - _time;
		
		if (i % 10 == 0) std::cout << i / 10 << "%" << std::endl;
	}

	double _time_sr = 0.;
	for (long long& t : time_command)
	{
		_time_sr += t;
	}*/

	//std::cout << "time :" << _time_sr / count_iter << std::endl;

	std::deque<scada_ate::gate::adapter::SetTags>* res = nullptr;

	adapter_dds_sub->ReadData(&res);

	

	std::cout << "priem " << std::endl;

	Sleep(1000);

	return 0;
}