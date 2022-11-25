#pragma once
#pragma once
#ifndef _CONFIG_UTILS_HPP_
#define _CONFIG_UTILS_HPP_

#include <memory>
#include <string>
#include <fstream>

#include "adapters.h"
#include "topics.h"
#include "LoggerConfig.h"
#include "../../../thirdparty/nlohmann-json/json.hpp"
#include "../../../lib/json_xml/include/Exception.hpp"

namespace atech {
    namespace common {

        size_t getConfigSize(const nlohmann::ordered_json& src,const std::string& topic_name, const std::string& type_name);

        std::vector<InputUnit> getInputUnits(const nlohmann::ordered_json& src, const std::string& type_name);
        std::vector<OutputUnit> getOutputUnits(const nlohmann::ordered_json& src, const std::string& type_name);


    } // namespace common
} //namespace atech

#endif _CONFIG_UTILS_HPP_

