#include <fstream>
#include "config/json.hpp"
#include "config/adapters.h"
#include "config/MessageSerializer.h"
#include <iostream>


int main()
{

	std::fstream file_in("config\\config.json", std::ios_base::in);
	nlohmann::ordered_json json = nlohmann::json::parse(file_in);

	json["adapters"]["units"].clear();

	{
		atech::common::Unit unit;
		std::vector<atech::common::InputUnit> unit_in_vec{};
		std::vector<atech::common::OutputUnit> unit_out_vec{};
		std::vector<atech::common::Mapping> map_vec{};
		unit.set_id(100);
		unit.set_frequency(1000);

		{
			atech::common::InputUnit unit_in;
			std::shared_ptr<atech::common::DdsConfig> dds_unit = std::make_shared<atech::common::DdsConfig>();
			dds_unit->set_topic_name("from_emt");
			dds_unit->set_type_name("DDSData");;
			unit_in.set_id(1);
			unit_in.set_id_map(1);
			unit_in.set_type_adapter("DDS");
			unit_in.set_version("");
			unit_in.set_config(dds_unit);
			unit_in_vec.push_back(unit_in);
		}

		{
			atech::common::OutputUnit unit_out;
			std::shared_ptr<atech::common::WsConfig> ws_unit = std::make_shared<atech::common::WsConfig>();
			std::vector<atech::common::Topic> vec_topic(1);
			std::vector<size_t> vec_tag(3000);
			std::iota(vec_tag.begin(), vec_tag.end(), 1);
			vec_topic[0].set_tag_ids(vec_tag);
			vec_topic[0].set_topic_name("from_emt");
			 
			ws_unit->set_host("127.0.0.1");
			ws_unit->set_is_binary_format(false);
			ws_unit->set_is_ipv4(true);
			ws_unit->set_period(1000);
			ws_unit->set_port(8081);
			ws_unit->set_send_changes_only(false);
			ws_unit->set_topics(vec_topic);
			unit_out.set_id(2);
			unit_out.set_id_map(1);
			unit_out.set_type_adapter("WS");
			unit_out.set_version("");
			unit_out.set_config(ws_unit);
			unit_out_vec.push_back(unit_out);
		}

		{
			atech::common::Mapping map;
			map.set_id_map(1);
			map.set_frequency(1000);

			{
				std::vector<atech::common::Datum> datum_vec{};
				for (int i = 0; i < 1000; i++)
				{
					atech::common::Datum datum;
					atech::common::Putdata in_data;
					atech::common::Putdata out_data;
					datum.set_delta(0);
					datum.set_type_registration("");

					in_data.set_tag("");
					in_data.set_id_tag(i+1);
					in_data.set_is_array(false);
					in_data.set_mask(0);
					in_data.set_offset(i);
					in_data.set_type("DATA_INT");

					out_data.set_tag("");
					out_data.set_id_tag(i+1);
					out_data.set_is_array(false);
					out_data.set_mask(0);
					out_data.set_offset(i);
					out_data.set_type("DATA_INT");

					datum.set_inputdata(in_data);
					datum.set_outputdata(out_data);

					datum_vec.push_back(datum);
				}

				for (int i = 0; i < 2000; i++)
				{
					atech::common::Datum datum;
					atech::common::Putdata in_data;
					atech::common::Putdata out_data;
					datum.set_delta(0);
					datum.set_type_registration("");

					in_data.set_tag("");
					in_data.set_id_tag(1000 + i + 1);
					in_data.set_is_array(false);
					in_data.set_mask(0);
					in_data.set_offset(i);
					in_data.set_type("DATA_FLOAT");

					out_data.set_tag("");
					out_data.set_id_tag(1000 + i + 1);
					out_data.set_is_array(false);
					out_data.set_mask(0);
					out_data.set_offset(i);
					out_data.set_type("DATA_FLOAT");

					datum.set_inputdata(in_data);
					datum.set_outputdata(out_data);

					datum_vec.push_back(datum);
				}

				map.set_data(datum_vec);
			}

			map_vec.push_back(map);
		}

		unit.set_input_units(unit_in_vec);
		unit.set_output_units(unit_out_vec);
		unit.set_mapping(map_vec);
		nlohmann::ordered_json j;
		nlohmann::adl_serializer<atech::common::Unit>::to_json(j, unit);
		json["adapters"]["units"].push_back(j);
	};


	std::fstream file_out("config.json", std::ios_base::out | std::ios_base::trunc);

	file_out << json.dump(2);
	file_in.close();
	file_out.close();
}