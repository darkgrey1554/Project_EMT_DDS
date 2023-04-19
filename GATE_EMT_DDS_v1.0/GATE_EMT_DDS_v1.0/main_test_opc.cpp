#include<Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include<Module_IO/UnitTransfer/adapters/OPCUA_Adapter/AdapterOPCUA.h>
#include<Module_IO/UnitTransfer/adapters/DDS_Adapter/AdapterDDS.hpp>

using namespace std;

namespace ate = scada_ate::gate::adapter;

char q = '0';

#define	LOG_NO_WARNING ON;
#define	LOG_NO_ERROR ON;
#define	LOG_NO_INFO ON;

void fun_out()
{
	while (q != 'q') std::cin>>q;
}

int main()
{

	size_t size_data_int = 100;
	size_t size_data_float = 100;
	std::vector<ate::InfoTag> vec_tag_source;
	std::vector<ate::InfoTag> vec_tag_source_dds;
	std::vector<ate::LinkTags> vec_link_tags;
	vec_tag_source.reserve(20);
	vec_tag_source_dds.reserve(20);
	vec_link_tags.reserve(20);
	for (size_t i = 0; i < 10; i++)
	{
		vec_tag_source.push_back({ "", 1001, 1, i, ate::TypeValue::INT,0,i });
		vec_tag_source.push_back({ "", 1003, 1, i, ate::TypeValue::FLOAT,0,i });
	}

	int counter_tag = 0;

	for (auto it : vec_tag_source)
	{
		vec_link_tags.push_back({ it, { "", counter_tag, 0, it.offset, it.type,0,it.offset_store}, ate::TypeRegistration::RECIVE, 0. });
		counter_tag++;
	}

	for (auto it : vec_link_tags)
	{
		vec_tag_source_dds.push_back(it.target);
	}

	
	std::deque<ate::SetTags>* data;
	std::shared_ptr<ate::opc::ConfigAdapterOPCUA> config_opc = std::make_shared<ate::opc::ConfigAdapterOPCUA>();
	std::shared_ptr<ate::dds::ConfigAdapterDDS> config_dds_pub = std::make_shared<ate::dds::ConfigAdapterDDS>();
	std::shared_ptr<ate::dds::ConfigAdapterDDS> config_dds_sub = std::make_shared<ate::dds::ConfigAdapterDDS>();

	atech::common::SizeTopics::SetMaxSizeDataCollectionInt(100);
	atech::common::SizeTopics::SetMaxSizeDataCollectionFloat(100);

	config_dds_pub->id_adapter = 11;
	config_dds_pub->str_config_ddslayer = "";
	config_dds_pub->topic_name = "test_topic_1";
	config_dds_pub->type_adapter = ate::TypeAdapter::DDS;
	config_dds_pub->type_data = ate::dds::TypeDDSData::DDSData;
	config_dds_pub->type_transfer = ate::dds::TypeTransfer::PUBLISHER;
	config_dds_pub->vec_link_tags = vec_link_tags;

	config_dds_sub->id_adapter = 12;
	config_dds_sub->str_config_ddslayer = "";
	config_dds_sub->topic_name = "test_topic_1";
	config_dds_sub->type_adapter = ate::TypeAdapter::DDS;
	config_dds_sub->type_data = ate::dds::TypeDDSData::DDSData;
	config_dds_sub->type_transfer = ate::dds::TypeTransfer::SUBSCRIBER;
	config_dds_sub->vec_tags_source = vec_tag_source_dds;


	config_opc->authentication = ate::opc::Authentication::Anonymous;
	config_opc->endpoint_url = "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer";
	config_opc->id_adapter = 10;
	config_opc->namespaceindex = 3;
	config_opc->password = "";
	config_opc->security_mode = ate::opc::SecurityMode::Sign;
	config_opc->security_policy = ate::opc::SecurityPolicy::Basic256;
	config_opc->type_adapter = ate::TypeAdapter::OPC_UA;
	config_opc->user_name = "";
	config_opc->vec_tags_source = vec_tag_source;

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> adapte_opc = scada_ate::gate::adapter::CreateAdapter(config_opc);
	std::shared_ptr<scada_ate::gate::adapter::IAdapter> adapte_dds_pub = scada_ate::gate::adapter::CreateAdapter(config_dds_pub);
	std::shared_ptr<scada_ate::gate::adapter::IAdapter> adapte_dds_sub = scada_ate::gate::adapter::CreateAdapter(config_dds_sub);

	adapte_opc->InitAdapter();
	adapte_dds_pub->InitAdapter();
	adapte_dds_sub->InitAdapter();
	adapte_opc->ReadData(&data);

	std::deque<ate::SetTags>& a = *data;

	int counter = 0;
	std::thread thread_out(fun_out);
	while (1)
	{
		adapte_opc->ReadData(&data);

		adapte_dds_pub->WriteData(*data);

		adapte_dds_sub->ReadData(&data);
																							   
		a = *data;

		/*for (auto it = a.begin(); it != a.end(); it++)
		{
			for (auto& it_map : it->map_data)
			{
				std::cout << "tag: " << it_map.first.id_tag << " index: " << it_map.first.offset << " value: ";
				if (it_map.first.type == ate::TypeValue::INT)
				{
					std::cout << std::get<int>(it_map.second.value) << std::endl;
				}
				else if (it_map.first.type == ate::TypeValue::FLOAT)
				{
					std::cout << std::get<float>(it_map.second.value) << std::endl;
				}
				else if (it_map.first.type == ate::TypeValue::DOUBLE)
				{
					std::cout << std::get<double>(it_map.second.value) << std::endl;
				}
			}
		}*/

		Sleep(1000);
		if (q == 'q') break;
	}

	thread_out.join();
	return 0;
}