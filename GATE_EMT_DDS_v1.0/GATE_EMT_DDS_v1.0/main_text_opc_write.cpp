#include<Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include<Module_IO/UnitTransfer/adapters/OPCUA_Adapter/AdapterOPCUA.h>

using namespace std;

namespace ate = scada_ate::gate::adapter;

char q = '0';

#define	LOG_NO_WARNING ON;
#define	LOG_NO_ERROR ON;
#define	LOG_NO_INFO ON;

void fun_out()
{
	while (q != 'q') std::cin >> q;
}

int main()
{
	std::vector<ate::InfoTag> vec_tag_target;
	std::vector<ate::InfoTag> vec_tag_source;
	std::vector<ate::LinkTags> vec_link_tags;
	vec_tag_target.reserve(23);
	vec_tag_source.reserve(23);
	vec_link_tags.reserve(23);
	for (size_t i = 0; i < 10; i++)
	{
		vec_tag_target.push_back({ "", 1001, 1, i, ate::TypeValue::INT });
		vec_tag_source.push_back({ "", (int)i, 1, i, ate::TypeValue::INT });
		vec_tag_target.push_back({ "", 1003, 1, i, ate::TypeValue::FLOAT });
		vec_tag_source.push_back({ "", (int)i+10, 1, i, ate::TypeValue::FLOAT });
	}

	vec_tag_target.push_back({ "", 1007, 0, 0, ate::TypeValue::INT });
	vec_tag_source.push_back({ "", 100, 0, 0, ate::TypeValue::INT });
	vec_tag_target.push_back({ "", 1008, 0, 0, ate::TypeValue::INT });
	vec_tag_source.push_back({ "", 101, 0, 0, ate::TypeValue::INT });
	vec_tag_target.push_back({ "", 1009, 0, 0, ate::TypeValue::INT });
	vec_tag_source.push_back({ "", 102, 0, 0, ate::TypeValue::INT });

	for (int i=0; i<23;i++)
	{
		vec_link_tags.push_back({ vec_tag_source[i], vec_tag_target[i], ate::TypeRegistration::RECIVE, 0.});
	}

	std::shared_ptr<ate::opc::ConfigAdapterOPCUA> config_opc = std::make_shared<ate::opc::ConfigAdapterOPCUA>();

	config_opc->authentication = ate::opc::Authentication::Anonymous;
	config_opc->endpoint_url = "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer";
	config_opc->id_adapter = 10;
	config_opc->namespaceindex = 3;
	config_opc->password = "";
	config_opc->security_mode = ate::opc::SecurityMode::Sign;
	config_opc->security_policy = ate::opc::SecurityPolicy::Basic256;
	config_opc->type_adapter = ate::TypeAdapter::OPC_UA;
	config_opc->user_name = "";
	config_opc->vec_tags_source.empty();
	config_opc->vec_link_tags = vec_link_tags;

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> adapte_opc = scada_ate::gate::adapter::CreateAdapter(config_opc);

	adapte_opc->InitAdapter();


	std::deque<ate::SetTags> data;
	{
		ate::SetTags _d{};
		for (auto& it : vec_tag_source)
		{
			if (it.type == ate::TypeValue::INT)
			{
				_d.map_data[it].value = 0;
				_d.map_data[it].quality = 0;
			}

			if (it.type == ate::TypeValue::FLOAT)
			{
				_d.map_data[it].value = 0.f;
				_d.map_data[it].quality = 0;
			}
		}

		data.push_back(_d);
	}

	int counter = 0;
	std::thread thread_out(fun_out);
	while (1)
	{

		for (auto& it : data.begin()->map_data)
		{
			if (it.first.type == ate::TypeValue::INT)
			{
				it.second.value = std::get<int>(it.second.value) + 1;
				it.second.quality = 0;
				it.second.time = TimeConverter::GetTime_LLmcs();
			}

			if (it.first.type == ate::TypeValue::FLOAT)
			{
				it.second.value = std::get<float>(it.second.value) + 0.1f;
				it.second.quality = 0;
				it.second.time = TimeConverter::GetTime_LLmcs();
			}
		}

		adapte_opc->WriteData(data);

		Sleep(2000);
		if (q == 'q') break;
	}

	thread_out.join();
	return 0;
}