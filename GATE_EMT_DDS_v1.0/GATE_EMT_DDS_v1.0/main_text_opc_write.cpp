#include<Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include<Module_IO/UnitTransfer/adapters/OPCUA_Adapter/AdapterOPCUA.h>
#include<config/MessageSerializer.h>
#include <sstream>

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

string j = R"({
 "node_source": 11,
 "node_target": 12,

  "dds": {
    "hash": 123123, 
    "profiles": {

      "transport_descriptors": {
        "transport_descriptor": [
          {
            "transport_id": "tcp2_transport",
            "type": "TCPv4",
            "sendBufferSize": 9216,
            "receiveBufferSize": 9216,
            "listening_ports": {
              "port": 5100
            },
            "wan_addr": "80.80.99.45"
          },
          {
            "transport_id": "tcp3_transport",
            "type": "TCPv4"
          }
        ]
      },

      "participant": {
        "-profile_name": "participant_profile",
        "domeinId": "0",
        "rtps": {},
        "name": {},
        "defaultUnicastLocatorList": {},
        "defaultMulticastLocatorList": {},
        "sendSocketBufferSize": {},
        "listenSocketBufferSize": {},
        "builtin": {
          "initialPeersList": {
            "locator": [
              {
                "tcpv4": {
                  "port": 5000,
                  "address": "192.168.0.10"
                }
              },
              {
                "tcpv4": {
                  "port": 5000,
                  "address": "192.168.0.11"
                }
              }
            ]
          }
        },
        "port": {},
        "participantID": {},
        "throughputController": {},
        "userTransports": {
          "transport_id": [
            "tcp2_transport",
            "tcp3_transport"
          ]
        },
        "useBuiltinTransports": false,
        "propertiesPolicy": {},
        "allocation": {}
      },

      "datawriter": [
        {
          "-profile_name": "datawrite_topic1_profile",
          "qos": {},
          "historyMemoryPolicy": {}
        },
        {
          "-profile_name": "datawrite_topic2_profile",
          "qos": {},
          "historyMemoryPolicy": {}
        }
      ],

      "datareader": [
        {
          "-profile_name": "datawreader_topic1_profile",
          "qos": {},
          "historyMemoryPolicy": {}
        },
        {
          "-profile_name": "datawreader_topic2_profile",
          "qos": {},
          "historyMemoryPolicy": {}
        }
      ],

      "topic": [
        {
          "-profile_name": "topic1_profile",
          "name": "",
          "dataType": "",
          "historyQos": {},
          "resourceLimitQos": {}
        },
        {
          "-profile_name": "topic2_profile",
          "name": "",
          "dataType": "",
          "historyQos": {},
          "resourceLimitQos": {}
        }
      ]
    }
  },

 "topic_max_size": {
      "hash":12345,
      "dds_type_size": [
        {
          "type_name": "DDSData", 
          "type_sizes": [
            {
              "type_name": "float",
              "size": 10
            },
            {
              "type_name": "int",
              "size": 5
            },

            {
              "type_name": "double",
              "size": 5
            },

            {
              "type_name": "char_vector",
              "size": 5
            },

            {
              "type_name": "char",
              "size": 5
            }
          ]
        },
         {
          "type_name": "DDSDataEx", 
          "type_sizes": [
            {
              "type_name": "float",
              "size": 10
            },
            {
              "type_name": "int",
              "size": 5
            },

            {
              "type_name": "double",
              "size": 5
            },

            {
              "type_name": "char_vector",
              "size": 5
            },

            {
              "type_name": "char",
              "size": 5
            }
          ]
        },
        {
          "type_name": "DDSAlarm", 
          "type_sizes": [
            {
              "type_name": "long",
              "size": 10
            }
          ]
        },
        {
          "type_name": "DDSAlarmEx", 
          "type_sizes": [
            {
              "type_name": "char",
              "size": 10
            }
          ]
        }
      ]
    },
 "adapters":
  {
    "hash": 123214, 
    "units": [ 
      {
        "id": 1, 
		"frequency" : 100,
        "input_units": [
		{
         "id": 1, 
          "type_adapter": "SM",
           "config": {
            "name_point_sm": "", 
            "size_int_data": 0, 
            "size_float_data": 0, 
            "size_double_data": 0, 
            "size_char_data": 0, 
            "size_str": 0
          },
		   "id_map": 0
        }
		],

        "output_units": [
          {
            "id": 1, 
			"type_adapter": "DDS",
            "config": {
              "topic_name": "name_",
              "type_name": "name_"
            },
            "id_map": 1
          },
          {
            "id": 2,
			"type_adapter": "SM",
             "config": {
              "name_point_sm": "", 
              "size_int_data": 0, 
              "size_float_data": 0, 
              "size_double_data": 0, 
              "size_char_data": 0, 
              "size_str": 0
            },
            "id_map": 2
          },
          {
            "id": 3, 
			"type_adapter": "OPC_UA",
            "config": {
              "endpoint_url": "", 
              "security_mode": "",
              "security_policy": "", 
              "user_name": "", 
              "password": "", 
              "namespace_index": ""
            },
            "id_map": 2
          }
        ],
       
        "mapping": [ 
          {
            "id_map": 1,
            "frequency": 100, 
            "data": [
              {
                "inputdata": {
                  "tag": "",
                  "id_tag": 1,
                  "is_array": false,
                  "offset": 0, 
                  "type": "",
                  "mask": 0 
                },

                "outputdata": {
                  "tag": "",
                  "id_tag": 1,
                  "is_array": false,
                  "offset": 0,
                  "type": "", 
                  "mask": 0 
                },

                "type_registration": "", 
                "delta": 0.0
              }
            ]
          }
        ]
      },
      {
		"id": 2, 
		"frequency" : 100,
        "input_units": [
		{
           "id": 1, 
			"type_adapter": "DDS",
            "config": {
              "topic_name": "data_1",
              "type_name": "DDSData"
            },
            "id_map": 1
        },
		{
           "id": 2, 
			"type_adapter": "DDS",
            "config": {
              "topic_name": "data_extended_1",
              "type_name": "DDSDataEx"
            },
            "id_map": 2
        }
		],
		  "output_units": [
          {
            "id": 3, 
			"type_adapter": "WS",
            "config": {
               "host": "127.0.0.1", 
               "port": 8081, 
               "period_ms": 1000, 
               "send_changes_only": false, 
			   "topics": [
				  {
					"topic_name": "data_1",
					"tag_ids": [ 2, 3, 5 ] 
				  },
				  {
					"topic_tame": "data_extended_1",
					"tag_ids": [ 12, 13, 22 ]
				  }
                ],
            "is_ipv4": true,
            "is_binary_format": false 
            },
            "id_map": 1
          }
		  ],
		  "mapping": [ 
          {
            "id_map": 1,
            "frequency": 100, 
            "data": [
              {
                "inputdata": {
                  "tag": "",
                  "id_tag": 1,
                  "is_array": false,
                  "offset": 0, 
                  "type": "",
                  "mask": 0 
                },
                "outputdata": {
                  "tag": "",
                  "id_tag": 1,
                  "is_array": false,
                  "offset": 0,
                  "type": "", 
                  "mask": 0 
                },

                "type_registration": "", 
                "delta": 0.0
              }
            ]
          }
        ]
          
      }
      ]
   }
 })";



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