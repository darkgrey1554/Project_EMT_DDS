#include "ConfigUtils.h"
#include "MessageSerializer.h"

namespace atech {
    namespace common {

        size_t getConfigSize(const nlohmann::ordered_json& src, const std::string& topic_name, const std::string& type_name)
        {
            try {
                // определение размера топика Config (сделать общую функцию  template)
                auto topic_max_size = src["topic_max_size"];
                auto dds_type_sizes = topic_max_size.get<TopicMaxSize>().get_dds_type_size();
                auto it = std::find_if(dds_type_sizes.begin(), dds_type_sizes.end(), [&topic_name](const atech::common::DdsTypeSize& elem) { return elem.get_type_name() == topic_name; });
                if (it != std::end(dds_type_sizes)) {
                    auto ts = it->get_type_sizes();
                    auto its = std::find_if(ts.begin(), ts.end(), [&type_name](const atech::common::TypeSize& elem) { return elem.get_type_name() == type_name; });
                    return its->get_size();
                }
                else
                    return -1;
            }
            catch (nlohmann::json::exception& e) {

                throw atech::common::Error("Error in topic_max_size of function  getConfigSize)  " + (std::string)e.what());
            }
            catch (std::exception& e) {

                throw atech::common::Error("Error in function  getConfigSize)  " + (std::string)e.what());
            }
            
        }

        std::vector<InputUnit> getInputUnits(const nlohmann::ordered_json& src, const std::string& type_name)
        {
            try {
                auto adapter_json = src["adapters"];
                std::string unit_name = type_name;
                std::vector<atech::common::InputUnit> input_units;
                auto units_ = adapter_json.get<atech::common::Adapters>().get_units();
                for (auto unit : units_)
                {
                    auto inputs = unit.get_input_units();
                    auto in = std::find_if(inputs.begin(), inputs.end(), [&unit_name](const atech::common::InputUnit& elem) { return elem.get_type_adapter() == unit_name; });
                    if (in != std::end(inputs)) {
                        input_units.push_back(*in);
                    }


                }
                
                return input_units;
            }
            catch (nlohmann::json::exception& e) {

                throw atech::common::Error("Error in adapters of function  getInputUnits)  " + (std::string)e.what());
            }
            catch (std::exception& e) {

                throw atech::common::Error("Error in function  getInputUits)  " + (std::string)e.what());
            }
        }

        std::vector<OutputUnit> getOutputUnits(const nlohmann::ordered_json& src, const std::string& type_name)
        {
            try {
                auto adapter_json = src["adapters"];
                std::string unit_name = type_name;
                std::vector<atech::common::OutputUnit> out_units;
                auto units_ = adapter_json.get<atech::common::Adapters>().get_units();
                for (auto unit : units_)
                {
                    auto outputs = unit.get_output_units();
                    auto out = std::find_if(outputs.begin(), outputs.end(), [&unit_name](const atech::common::OutputUnit& elem) { return elem.get_type_adapter() == unit_name; });
                    if (out != std::end(outputs)) {
                        out_units.push_back(*out);
                    }
                    else
                    {
                        ;// std::cout << "Outputs unit is not found" << std::endl;
                    }
                }
                return out_units;
            }
            catch (nlohmann::json::exception& e) {

                throw atech::common::Error("Error in adapters of function  getOutputUnits)  " + (std::string)e.what());
            }
            catch (std::exception& e) {

                throw atech::common::Error("Error in function  getOutputtUits)  " + (std::string)e.what());
            }
        }

    } // namespace common
} //namespace atech
