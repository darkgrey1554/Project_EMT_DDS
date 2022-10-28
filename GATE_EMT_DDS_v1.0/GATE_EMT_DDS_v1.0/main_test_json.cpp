#include <json2xml.hpp>
#include <MessageSerializer.h>
#include <json.hpp>
#include <tinyxml2.h>


#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>


//using json = nlohmann::json;
//using namespace tinyxml2;
using namespace std;

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

 "topic_defualt_size": {
      "hash":12345,
      "config_types": [
        {
          "type_name": "DDSData", 
          "sizes": [
            {
              "type": "float",
              "size": 10
            },
            {
              "type": "int",
              "size": 5
            },

            {
              "type": "double",
              "size": 5
            },

            {
              "type": "char_vector",
              "size": 5
            },

            {
              "type": "char",
              "size": 5
            }
          ]
        }
      ]
    },
  

  "topic_size": { 
    "hash": 12345,
    "topics": [
      {
        "name": "", 
        "sizes": [
          {
            "type": "float",
            "size": 10
          },
          {
            "type": "int",
            "size": 5
          },

          {
            "type": "double",
            "size": 5
          },

          {
            "type": "char_vector",
            "size": 5
          },

          {
            "type": "char",
            "size": 5
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
        "frequency_unit" : 100,
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
          }
        }
        ],

        "output_units": [
          {
            "id": 1,
            "type_adapter": "DDS",
            "config": 
            {
              "topic_name": "name_"
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
          }
        ],
       

        "mapping": [ 
          {
            "id_map": 1,
            "frequency": 100, 
            "data": [
              {
                "tag": "", 
                "is_array": false, 
                "offset": 0, 
                "type": "", 
                "mask": "", 

                "id_tag": "",
                "type_value": "", 
                "offset_topic": 0, 
                "mask_topic": "", 
                "type_registration": "", 
                "delta": 2.3
              }
            ]
          },
          {
            "id_map": 2,
            "frequency": 200, 
            "data": [
              {
                "tag": "", 
                "is_array": false, 
                "offset": 0, 
                "type": "", 
                "mask": "", 

                "id_tag": "", 
                "type_value": "", 
                "offset_topic": 0, 
                "mask_topic": "", 
                "type_registration": "", 
                "delta": 0.0
              }
            ]
          }
        ]
      },
      {

      }
    ]
  }
})";




std::string str = R"(<?xml version="1.0" encoding="UTF-8" ?>
<dds>
    <profiles xmlns="http://www.eprosima.com/XMLSchemas/fastRTPS_Profiles">
        <participant profile_name = "participant_profile">
        </participant>
        <publisher profile_name="test_publisher_profile">
        </publisher>
        <data_writer profile_name = "datawriter_profile">
        </data_writer>    
        <data_reader profile_name = "datareader_profile">   
        </data_reader>    
        <transport_descriptors> 
        </transport_descriptors>  
    </profiles>
</dds>)";

std::string str2 = "\<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\
<dds>\
    <profiles xmlns=\"http://www.eprosima.com/XMLSchemas/fastRTPS_Profiles\" >\
        <publisher profile_name=\"test_publisher_profile\" is_default_profile=\"true\">\
            <qos>\
                <durability>\
                    <kind>TRANSIENT_LOCAL</kind>\
                </durability>\
            </qos>\
        </publisher>\
    </profiles>\
</dds>\
";

std::string str3 = R"(<?xml version="1.0" encoding="UTF-8" ?>
<dds>
    <profiles xmlns="http://www.eprosima.com/XMLSchemas/fastRTPS_Profiles\" >
        <publisher profile_name="test_publisher_profile">
            <qos>
                <durability>
                    <kind>TRANSIENT_LOCAL</kind>
                </durability>
            </qos>
        </publisher>
    </profiles>
</dds>
)";

std::string str4 = R"(<?xml version="1.0" encoding="UTF-8" ?>
<dds>
    <profiles xmlns="http://www.eprosima.com/XMLSchemas/fastRTPS_Profiles">
        <publisher profile_name="test_publisher_profile">
        </publisher>
        <topic profile_name="topic_xxx">
            <kind>NO_KEY</kind>
            <name>topic_xxx</name>
            <dataType>DDSData</dataType>
        </topic>
    </profiles>
</dds>)";


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::types;
using namespace std::chrono_literals;

int main()
{

    {
        std::cout << "asd" << std::endl;
             
        std::cout << "asd" << std::endl;
    }

    setlocale(LC_ALL, "rus");
    nlohmann::json json_data = nlohmann::json::parse(j);

    auto str = json_data["dds"]["profiles"];
    auto hash = std::hash<json>{}(json_data["dds"]["profiles"]);

    return 0;
}



