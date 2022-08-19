#include<Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include<Module_IO/UnitTransfer/adapters/OPCUA_Adapter/AdapterOPCUA.h>

using namespace std;
namespace ate = scada_ate::gate::adapter;

int main()
{

	std::vector<ate::InfoTag> vec_tag_source;
	vec_tag_source.push_back({ "", 1001, 1, 0, ate::TypeValue::INT});
	vec_tag_source.push_back({ "", 1002, 0, 0, ate::TypeValue::INT });
	vec_tag_source.push_back({ "", 1003, 0, 0, ate::TypeValue::FLOAT });
	std::deque<ate::SetTags>* data;
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
	config_opc->vec_tags_source = vec_tag_source;
	config_opc->vec_link_tags;

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> adapte = scada_ate::gate::adapter::CreateAdapter(config_opc);

	adapte->InitAdapter();
	adapte->ReadData(&data);

	std::deque<ate::SetTags>& a = *data;



	return 0;
}