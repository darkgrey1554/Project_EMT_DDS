#include "ReaderConfigUnits.hpp"

namespace scada_ate
{
	namespace module_io
	{

        ReaderConfigUnits::ReaderConfigUnits()
        {
            log = LoggerSpace::Logger::getpointcontact();
            return;
        };

        ReaderConfigUnits::~ReaderConfigUnits()
        {
            return;
        };

		ResultReqest  ReaderConfigUnits::CheckConfig(std::string file_name)
		{
            std::ifstream file;
            std::string help_str;
            ResultReqest res = ResultReqest::OK;
            ConfigDDSUnit config_unit;


            /// --- parsing configfile with rapidjson lib --- ///
            try
            {
                /// --- open file name_config (default = config.json) --- ////
                file.open(file_name, std::ios::in);
                if (!file.is_open())
                {
                    throw 1;
                }
                /// --- reading the whole file --- ///
                std::string str((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

                rapidjson::Document document;
                document.Parse(str.c_str());

                if (!document.HasMember("Units")) throw 2;
                if (!document["Units"].IsArray()) throw 3;
                if (document["Units"].Size() <= 0) throw 4;

                for (int i = 0; i < document["Units"].Size(); i++)
                {
                    if (take_domen(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_typeunit(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_typetransmite(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_typeadapter(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_pointname(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_typedata(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_size(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_frequency(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;

                    if (config_unit.Transmiter != TypeTransmiter::Broadcast)
                    {
                        if (take_portmain(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                        if (take_ipmain(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                        if (take_portreserve(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                        if (take_ipreserve(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    }             
                }
                

            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: ", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits:", 0, 0);
                res = ResultReqest::ERR;
            }

            file.close();
            return res;
		}

        ResultReqest ReaderConfigUnits::CheckNewConfig()
        {
            ResultReqest res;
            res = CheckConfig(name_configunits_new);
            return res;
        }

        ResultReqest ReaderConfigUnits::CheckBaseConfig()
        {
            ResultReqest res;
            res = CheckConfig(name_configunits);
            return res;
        }

        ResultReqest ReaderConfigUnits::UpdateNewConfig(std::string std)
        {
            std::fstream file;

            ResultReqest res = ResultReqest::OK;

            try
            {
                file.open(name_configunits_new, std::ios::in | std::ios::trunc);
                if (!file.is_open()) throw 1;
                file << std;
                
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error UpdateNewConfig: ", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error UpdateNewConfig: ", 0, 0);
                res = ResultReqest::ERR;
            }

            file.close();
            return res;             
        };

        ResultReqest ReaderConfigUnits::UpdateBaseConfig()
        {
            ResultReqest res = ResultReqest::OK;

            try
            {
                if (!std::filesystem::exists(name_configunits_new)) throw 1;
                if (CheckNewConfig() != ResultReqest::OK) throw 2;

                std::filesystem::copy(name_configunits, name_configunits_old, std::filesystem::copy_options::overwrite_existing);
                std::filesystem::copy(name_configunits_new, name_configunits, std::filesystem::copy_options::overwrite_existing);
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error UpdateBaseConfig: ", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error UpdateBaseConfig: ", 0, 0);
                res = ResultReqest::ERR;
            }           

            return res;
        }

        ResultReqest ReaderConfigUnits::ReadConfig(std::vector<ConfigDDSUnit>& vector_result)
        {
            

            std::ifstream file;
            std::string help_str;
            ResultReqest res = ResultReqest::OK;
            ConfigDDSUnit config_unit;

            std::vector<ConfigDDSUnit> vector;

            /// --- parsing configfile with rapidjson lib --- ///
            try
            {

                if (CheckBaseConfig() != ResultReqest::OK) throw 1;

                /// --- open file name_config (default = config.json) --- ////
                file.open(name_configunits, std::ios::in);
                if (!file.is_open())
                {
                    throw 2;
                }
                /// --- reading the whole file --- ///
                std::string str((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

                rapidjson::Document document;
                document.Parse(str.c_str());

                if (!document.HasMember("Units")) throw 3;
                if (!document["Units"].IsArray()) throw 4;
                if (document["Units"].Size() <= 0) throw 5;

                for (int i = 0; i < document["Units"].Size(); i++)
                {
                    config_unit.Adapter = TypeAdapter::Null;
                    config_unit.Domen = 0;
                    config_unit.Frequency = 350;
                    config_unit.IP_MAIN.clear();
                    config_unit.IP_RESERVE.clear();
                    config_unit.PointName.clear();
                    config_unit.Port_MAIN = 0;
                    config_unit.Port_RESERVE = 0;
                    config_unit.Size = 0;
                    config_unit.Transmiter = TypeTransmiter::Broadcast;
                    config_unit.Typedata = TypeData::ZERO;
                    config_unit.TypeUnit = TypeDDSUnit::Empty;

                    if (take_domen(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_typeunit(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_typetransmite(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_typeadapter(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_pointname(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_typedata(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_size(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    if (take_frequency(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;

                    if (config_unit.Transmiter != TypeTransmiter::Broadcast)
                    {
                        if (take_portmain(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                        if (take_ipmain(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                        if (take_portreserve(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                        if (take_ipreserve(document, config_unit, i) != ResultReqest::OK) res = ResultReqest::ERR;
                    }

                    vector.push_back(config_unit);
                }

            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error ReadConfig: ", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits:  error ReadConfig: ", 0, 0);
                res = ResultReqest::ERR;
            }

            file.close();

            vector_result.clear();
            vector_result = vector;

            return res;

        };

        ResultReqest ReaderConfigUnits::take_domen(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;

            try
            {
                if (!doc["Units"][i].HasMember("Domen")) throw 1;
                if (!doc["Units"][i]["Domen"].IsUint()) throw 2;
                conf.Domen = doc["Units"][i]["Domen"].GetUint();
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error Domen", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error Domen", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_typeunit(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;
            std::string helpstr;

            try
            {
                if (!doc["Units"][i].HasMember("TypeUnit")) throw 1;
                if (!doc["Units"][i]["TypeUnit"].IsString()) throw 2;
                helpstr.clear();
                helpstr = doc["Units"][i]["TypeUnit"].GetString();
                if (StringToTypeUnit(helpstr, conf.TypeUnit) != ResultReqest::OK) throw 3;
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ConfigReader: error ReaderConfigMODULE_IO: error TypeTratsmiter", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ConfigReader: error ReaderConfigMODULE_IO: error TypeTratsmiter", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_typetransmite(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;
            std::string helpstr;

            try
            {
                if (!doc["Units"][i].HasMember("TypeTransmite")) throw 1;
                if (!doc["Units"][i]["TypeTransmite"].IsString()) throw 2;
                helpstr.clear();
                helpstr = doc["Units"][i]["TypeTransmite"].GetString();
                if (StringToTypeTransmiter(helpstr, conf.Transmiter) != ResultReqest::OK) throw 3;
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error TypeTratsmiter", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error TypeTratsmiter", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_typeadapter(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;
            std::string helpstr;

            try
            {
                if (!doc["Units"][i].HasMember("TypeAdapter")) throw 1;
                if (!doc["Units"][i]["TypeAdapter"].IsString()) throw 2;
                helpstr.clear();
                helpstr = doc["Units"][i]["TypeAdapter"].GetString();
                if (StringToTypeAdapter(helpstr, conf.Adapter) != ResultReqest::OK) throw 3;
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error TypeAdapter", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error TypeAdapter", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_pointname(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;
            std::string helpstr;

            try
            {
                if (!doc["Units"][i].HasMember("PointName")) throw 1;
                if (!doc["Units"][i]["PointName"].IsString()) throw 2;
                helpstr.clear();
                helpstr = doc["Units"][i]["PointName"].GetString();
                if (helpstr.empty()) throw 3;
                conf.PointName = helpstr;
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error PointName", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error PointName", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_typedata(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;
            std::string helpstr;

            try
            {
                if (!doc["Units"][i].HasMember("TypeData")) throw 1;
                if (!doc["Units"][i]["TypeData"].IsString()) throw 2;
                helpstr.clear();
                helpstr = doc["Units"][i]["TypeData"].GetString();
                if (StringToTypeData(helpstr, conf.Typedata) != ResultReqest::OK) throw 3;
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error TypeData", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error TypeData", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_size(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;

            try
            {
                if (!doc["Units"][i].HasMember("Size")) throw 1;
                if (!doc["Units"][i]["Size"].IsUint()) throw 2;
                conf.Size = doc["Units"][i]["Size"].GetUint();
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ConfigReader: error ReaderConfigUnits: error Size", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ConfigReader: error ReaderConfigUnits: error Size", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_frequency(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;

            try
            {
                if (!doc["Units"][i].HasMember("Frequency")) throw 1;
                if (!doc["Units"][i]["Frequency"].IsUint()) throw 2;
                conf.Frequency = doc["Units"][i]["Frequency"].GetUint();
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ConfigReader: error ReaderConfigUnits: error Frequency", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ConfigReader: error ReaderConfigUnits: error Frequency", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_portmain(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;

            try
            {
                if (!doc["Units"][i].HasMember("Port_Main")) throw 1;
                if (!doc["Units"][i]["Port_Main"].IsUint()) throw 2;
                conf.Port_MAIN = doc["Units"][i]["Port_Main"].GetUint();
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error Port_Main", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error Port_Main", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_ipmain(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;
            std::string helpstr;

            try
            {
                if (!doc["Units"].HasMember("IP_Main")) throw 1;
                if (!doc["Units"][i]["IP_Main"].IsString()) throw 2;
                helpstr = doc["Units"][i]["IP_Main"].GetString();
                if (CheckIP(helpstr) == ResultReqest::OK)
                {
                    conf.IP_MAIN.clear();
                    conf.IP_MAIN = helpstr;
                }
                else
                {
                    throw 3;
                }
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error IP_Main", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error IP_Main", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_portreserve(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;

            try
            {
                if (!doc["Units"][i].HasMember("Port_Reserve")) throw 1;
                if (!doc["Units"][i]["Port_Reserve"].IsUint()) throw 2;
                conf.Port_RESERVE = doc["Units"][i]["Port_Reserve"].GetUint();
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ConfigReader: error ReaderConfigUnits: error Port_Reserve", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ConfigReader: error ReaderConfigUnits: error Port_Reserve", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::take_ipreserve(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i)
        {
            ResultReqest res = ResultReqest::OK;
            std::string helpstr;

            try
            {
                if (!doc["Units"].HasMember("IP_Reserve")) throw 1;
                if (!doc["Units"][i]["IP_Reserve"].IsString()) throw 2;
                helpstr = doc["Units"][i]["IP_Reserve"].GetString();
                if (CheckIP(helpstr) == ResultReqest::OK)
                {
                    conf.IP_RESERVE.clear();
                    conf.IP_RESERVE = helpstr;
                }
                else
                {
                    throw 3;
                }
            }
            catch (int& e)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error IP_Reserve", e, 0);
                res = ResultReqest::ERR;
            }
            catch (...)
            {
                log->WriteLogWARNING("Error ReaderConfigUnits: error IP_Reserve", 0, 0);
                res = ResultReqest::ERR;
            }

            return res;
        }

        ResultReqest ReaderConfigUnits::StringToTypeUnit(std::string str, TypeDDSUnit& value)
        {
            if (str.compare("Publisher") == 0)
            {
                value = TypeDDSUnit::PUBLISHER;
                return ResultReqest::OK;
            }

            if (str.compare("Subscriber") == 0)
            {
                value = TypeDDSUnit::SUBSCRIBER;
                return ResultReqest::OK;
            }

            return ResultReqest::ERR;
        }

        ResultReqest ReaderConfigUnits::StringToTypeTransmiter(std::string str, TypeTransmiter& value)
        {

            if (str.compare("Broadcast") == 0)
            {
                value = TypeTransmiter::Broadcast;
                return ResultReqest::OK;
            }

            if (str.compare("TCP") == 0)
            {
                value = TypeTransmiter::TCP;
                return ResultReqest::OK;
            }

            if (str.compare("UDP") == 0)
            {
                value = TypeTransmiter::UDP;
                return ResultReqest::OK;
            }

            return ResultReqest::ERR;
        };

        ResultReqest ReaderConfigUnits::StringToTypeAdapter(std::string str, TypeAdapter& value)
        {

            if (str.compare("SharedMemory") == 0)
            {
                value = TypeAdapter::SharedMemory;
                return ResultReqest::OK;
            }

            if (str.compare("DDS") == 0)
            {
                value = TypeAdapter::DDS;
                return ResultReqest::OK;
            }

            if (str.compare("DTS") == 0)
            {
                value = TypeAdapter::DTS;
                return ResultReqest::OK;
            }

            if (str.compare("OPC_UA") == 0)
            {
                value = TypeAdapter::OPC_UA;
                return ResultReqest::OK;
            }

            if (str.compare("SMTP") == 0)
            {
                value = TypeAdapter::SMTP;
                return ResultReqest::OK;
            }

            return ResultReqest::ERR;
        };

        ResultReqest ReaderConfigUnits::StringToTypeData(std::string str, TypeData& value)
        {
            if (str.compare("Analog") == 0)
            {
                value = TypeData::ANALOG;
                return ResultReqest::OK;
            }

            if (str.compare("Discrete") == 0)
            {
                value = TypeData::DISCRETE;
                return ResultReqest::OK;
            }

            if (str.compare("Binar") == 0)
            {
                value = TypeData::BINAR;
                return ResultReqest::OK;
            }

            return ResultReqest::ERR;
        }

        ResultReqest ReaderConfigUnits::CheckIP(std::string str)
        {
            size_t first;
            size_t second;
            size_t last;

            if (std::count(str.begin(), str.end(), '.') != 3) return ResultReqest::ERR;

            first = str.find_first_of('.');
            second = str.find_first_of('.', first + 1);
            last = str.find_first_of('.', second + 1);

            if (str.substr(0, first).empty()) return ResultReqest::ERR;
            if (str.substr(first + 1, second - first - 1).empty()) return ResultReqest::ERR;
            if (str.substr(second + 1, last - second - 1).empty()) return ResultReqest::ERR;
            if (str.substr(last + 1).empty()) return ResultReqest::ERR;

            if (str.substr(0, first).find_first_not_of("0123456789") != std::string::npos) return ResultReqest::ERR;
            if (str.substr(first + 1, second - first - 1).find_first_not_of("0123456789") != std::string::npos) return ResultReqest::ERR;
            if (str.substr(second + 1, last - second - 1).find_first_not_of("0123456789") != std::string::npos) return ResultReqest::ERR;
            if (str.substr(last + 1).find_first_not_of("0123456789") != std::string::npos) return ResultReqest::ERR;

            if (std::stoul(str.substr(0, first)) > 255) return ResultReqest::ERR;
            if (std::stoul(str.substr(first + 1, second - first - 1)) > 255) return ResultReqest::ERR;
            if (std::stoul(str.substr(second + 1, last - second - 1)) > 255) return ResultReqest::ERR;
            if (std::stoul(str.substr(last + 1)) > 255) return ResultReqest::ERR;

            return ResultReqest::OK;
        }



	}
}