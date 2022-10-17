

#ifndef JSON_2_XML_H_
#define JSON_2_XML_H_

// Standard

#include<iostream>

// Nlohmann (https://github.com/nlohmann/json)
#include "json.hpp"
#include "tinyxml2.h"

namespace scd {
    namespace common {

        using json = nlohmann::json;
        using namespace tinyxml2;
        using namespace std;

        template<class UnaryFunction>
        void recursive_iterate(const json& j, XMLNode* pNode, XMLDocument* xmlDoc, UnaryFunction f)
        {


            for (auto it = j.begin(); it != j.end(); ++it)
            {

                if (it->is_object())
                {
                    XMLElement* pElement = xmlDoc->NewElement(it.key().c_str());
                    pNode->InsertEndChild(pElement);

                    recursive_iterate(*it, pElement, xmlDoc, f);
                }
                else if (it->is_primitive())
                {

                    if (!it.key().find_first_of('-'))
                    {
                        std::string value = it.value();
                        std::string key = it.key();

                        pNode->ToElement()->SetAttribute(key.erase(0, 1).c_str(), value.c_str());

                    }
                    else {
                        XMLElement* pElement = xmlDoc->NewElement(it.key().c_str());
                        f(it, pElement);
                        pNode->InsertEndChild(pElement);

                    }
                }
                else if (it->is_array())
                {

                    for (auto item = it->begin(); item != it->end(); ++item)
                    {
                        if (item->is_object())
                        {
                            XMLElement* pElement = xmlDoc->NewElement(it.key().c_str());
                            pNode->InsertEndChild(pElement);
                            recursive_iterate(*item, pElement, xmlDoc, f);
                        }
                        else if (item->is_primitive())// || it->is_array())
                        {
                            XMLElement* pElement = xmlDoc->NewElement(it.key().c_str());
                            f(item, pElement);
                            pNode->InsertEndChild(pElement);

                        }

                    }
                }

            }
        }

        std::string json2xml(const nlohmann::basic_json<>& src);
    }
}


#endif//_JSON_2_XML_H

