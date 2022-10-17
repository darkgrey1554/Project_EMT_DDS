#include "json2xml.hpp"

namespace scd {
	namespace common {

        std::string json2xml(const nlohmann::basic_json<>& src)
        {
            try {

                XMLDocument xmlDoc;
                XMLDeclaration* dec = xmlDoc.NewDeclaration();
                xmlDoc.InsertFirstChild(dec);
                XMLNode* pRoot = xmlDoc.NewElement("dds");
                xmlDoc.InsertEndChild(pRoot);
                pRoot->ToElement()->SetAttribute("xmlns", "http://www.eprosima.com/XMLSchemas/fastRTPS_Profiles");
                XMLElement* pElement = xmlDoc.NewElement("profiles");
                pRoot->InsertEndChild(pElement);

                recursive_iterate(src, pElement, &xmlDoc, [&](json::const_iterator it, XMLElement* pElement) {

                    if (it->is_number_float())
                        pElement->SetText(it.value().get<float>());
                    else if (it->is_number_integer())
                        pElement->SetText(it.value().get<int>());
                    else if (it->is_boolean())
                        pElement->SetText(it.value().get<bool>());
                    else if (it->is_string())
                        pElement->SetText(it.value().get<std::string>().c_str());

                    });
                //xmlDoc.SaveFile("SavedData.xml");
                XMLPrinter printer;
                pRoot->Accept(&printer);
                return printer.CStr();
            }
            catch (std::exception const& e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
            }

        }

	}
}