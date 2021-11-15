#include "Config_Reader.h"
#include <algorithm>

namespace scada_ate
{
    namespace service_io
    {
        namespace config
        {
            ///////////////////////////////////////////////////////////////
            // constrictor "ConfigReader" with default Logger
            ///////////////////////////////////////////////////////////////

            ConfigReader::ConfigReader()
            {
                log = LoggerSpace::Logger::getpointcontact();
            }

           
            ///////////////////////////////////////////////////////////////
            // read configuration header GATE in file config.json
            // conf - result read configuration
            // return - success of the operation
            ///////////////////////////////////////////////////////////////

            ResultReqest ConfigReader::ReadConfigGATE(ConfigGate& config)
            {

                std::ifstream file;
                std::string help_str;
                ResultReqest res = ResultReqest::OK;
                
                ConfigGate conf;
                conf.IdGate = 0;              

                /// --- parsing configfile with rapidjson lib --- ///
                try
                {
                    /// --- open file name_config (default = config.json) --- ////
                    file.open(name_config, std::ios::in);
                    if (!file.is_open())
                    {
                        throw 1;
                    }
                    /// --- reading the whole file --- ///
                    std::string str((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

                    rapidjson::Document document;
                    document.Parse(str.c_str());

                    if (!document.HasMember("GATE")) throw 2;
                    if (!document["GATE"].IsObject()) throw 3;

                    if (take_gate_idgate(document, conf) == ResultReqest::ERR) res = ResultReqest::ERR;                   

                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigGate:", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigGate:", 0, 0);
                    res = ResultReqest::ERR;
                }
                               
                file.close();
                config = conf;
                return res;
            }

            ResultReqest ConfigReader::take_gate_idgate(rapidjson::Document& doc, ConfigGate& conf)
            {
                ResultReqest res = ResultReqest::OK;

                try
                {
                    if (!doc["GATE"].HasMember("IdGate")) throw 4;
                    if (!doc["GATE"]["IdGate"].IsUint()) throw 5;
                    conf.IdGate = doc["GATE"]["IdGate"].GetUint();
                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigGate: erro IdGate ", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigGate: erro IdGate", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ///////////////////////////////////////////////////////////////
            // read configuration header LOGGER in file config.json
            // conf - result read configuration
            // return - success of the operation
            ///////////////////////////////////////////////////////////////

            ResultReqest ConfigReader::ReadConfigLOGGER(ConfigLogger& conf)
            {

                std::ifstream file;
                std::string help_str;
                ResultReqest res = ResultReqest::OK;

                ConfigLogger config;
                config.LogMode = LoggerSpace::LogMode::DEBUG;
                config.LogName = "log";
                config.SizeLogFile = 10;
                config.StatusLog = LoggerSpace::Status::OFF;
                config.StatusSysLog = LoggerSpace::Status::OFF;
                config.SysLogName = "log";

                /// --- parsing str with rapidjson lib --- /// 
                try
                {
                    /// --- open file name_config (config.json) --- ///
                    file.open(name_config, std::ios::in);
                    if (!file.is_open())
                    {
                        throw 1;
                    }

                    /// --- reading the whole file --- ///
                    std::string str((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

                    rapidjson::Document document;
                    document.Parse(str.c_str());

                    if (!document.HasMember("LOGGER")) throw 2;
                    if (!document["LOGGER"].IsObject()) throw 3;

                    if (take_logger_LogName(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (take_logger_SysLogName(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (take_logger_LogMode(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (take_logger_StatusLog(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (take_logger_StatusSysLog(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (take_logger_SizeLogFile(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                   
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER:", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER:", 0, 0);
                    res = ResultReqest::ERR;
                }


                file.close();
                conf = config;
                return res;
            };

            ResultReqest ConfigReader::take_logger_LogName(rapidjson::Document& doc, ConfigLogger& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["LOGGER"].HasMember("LogName")) throw 1;
                    if (!doc["LOGGER"]["LogName"].IsString()) throw 2;
                    helpstr = doc["LOGGER"]["LogName"].GetString();
                    if (helpstr.empty()) throw 3;
                    conf.LogName = helpstr;
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error LogName", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error LogName", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }            

            ResultReqest ConfigReader::take_logger_SysLogName(rapidjson::Document& doc, ConfigLogger& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["LOGGER"].HasMember("SysLogName")) throw 1;
                    if (!doc["LOGGER"]["SysLogName"].IsString()) throw 2;
                    helpstr = doc["LOGGER"]["LogName"].GetString();
                    if (helpstr.empty()) throw 3;
                    conf.SysLogName = helpstr;
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error SysLogName", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error SysLogName", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest ConfigReader::take_logger_LogMode(rapidjson::Document& doc, ConfigLogger& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["LOGGER"].HasMember("LogMode")) throw 1;
                    if (!doc["LOGGER"]["LogMode"].IsString()) throw 2;
                    helpstr = doc["LOGGER"]["LogMode"].GetString();
                    if (StringToLogMode(helpstr, conf.LogMode) == ResultReqest::ERR) throw 3;
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error LogMode", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error LogMode", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest ConfigReader::take_logger_StatusLog(rapidjson::Document& doc, ConfigLogger& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["LOGGER"].HasMember("StatusLog")) throw 1;
                    if (!doc["LOGGER"]["StatusLog"].IsString()) throw 2;
                    helpstr = doc["LOGGER"]["StatusLog"].GetString();
                    if (StringToLogStatus(helpstr, conf.StatusLog) == ResultReqest::ERR) throw 3;
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error StatusLog", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error StatusLog", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest ConfigReader::take_logger_StatusSysLog(rapidjson::Document& doc, ConfigLogger& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["LOGGER"].HasMember("StatusSysLog")) throw 1;
                    if (!doc["LOGGER"]["StatusSysLog"].IsString()) throw 2;
                    helpstr = doc["LOGGER"]["StatusSysLog"].GetString();
                    if (StringToLogStatus(helpstr, conf.StatusSysLog) == ResultReqest::ERR) throw 3;
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error StatusSysLog", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error StatusSysLog", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest ConfigReader::take_logger_SizeLogFile(rapidjson::Document& doc, ConfigLogger& conf)
            {
                ResultReqest res = ResultReqest::OK;

                try
                {
                    if (!doc["LOGGER"].HasMember("SizeLogFile")) throw 1;
                    if (!doc["LOGGER"]["SizeLogFile"].IsUint()) throw 2;
                    conf.SizeLogFile = doc["LOGGER"]["SizeLogFile"].GetUint();
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error SizeLogFile", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error SizeLogFile", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ///////////////////////////////////////////////////////////////
            // read configuration header LOGGER in file config.json
            // conf - result read configuration
            // return - success of the operation
            ///////////////////////////////////////////////////////////////

            ResultReqest ConfigReader::ReadConfigCONTROLLER_TCP(ConfigUnitCP_TCP& conf)
            {
                std::ifstream file;
                std::string help_str;
                ResultReqest res = ResultReqest::OK;
                /// --- open file name_config (config.json) --- ///
                ConfigUnitCP_TCP config;
                config.ip = "127.0.0.1";
                config.port = 32510;
                config.type_unit = TypeUnitCP::TCP;

                /// --- parsing str with rapidjson lib --- /// 
                try
                {
                    file.open(name_config, std::ios::in);
                    if (!file.is_open())
                    {
                        throw 1;
                    }

                    /// --- reading the whole file --- ///
                    std::string str((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

                    rapidjson::Document document;
                    document.Parse(str.c_str());

                    if (!document.HasMember("CONTROLLER_TCP")) throw 2;
                    if (!document["CONTROLLER_TCP"].IsObject()) throw 3;
                    
                    if (take_controllertcp_ip(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if(take_controllertcp_port(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;                    

                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigCONRTOLLER_TCP: error", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigCONRTOLLER_TCP: error", 0, 0);
                    res = ResultReqest::ERR;
                }

                file.close();
                conf = config;
                return res;
            }

            ResultReqest  ConfigReader::take_controllertcp_ip(rapidjson::Document& doc, ConfigUnitCP_TCP& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["CONTROLLER_TCP"].HasMember("IP")) throw 1;
                    if (!doc["CONTROLLER_TCP"]["IP"].IsString()) throw 2;
                    helpstr = doc["CONTROLLER_TCP"]["IP"].GetString();
                    if (CheckIP(helpstr) == ResultReqest::OK)
                    {
                        conf.ip.clear();
                        conf.ip = helpstr;
                    }
                    else
                    {
                        throw 3;
                    }
                    

                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigContreller_TCP: error IP", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigContreller_TCP: error IP", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_controllertcp_port(rapidjson::Document& doc, ConfigUnitCP_TCP& conf)
            {
                ResultReqest res = ResultReqest::OK;
                unsigned int port;

                try
                {
                    if (!doc["CONTROLLER_TCP"].HasMember("Port")) throw 1;
                    if (!doc["CONTROLLER_TCP"]["Port"].IsUint()) throw 2;
                    port = doc["CONTROLLER_TCP"]["Port"].GetUint();
                    if (port > 65535) throw 3;
                    conf.port = port;
                    
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error SizeLogFile", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigLOGGER: error SizeLogFile", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////
            ///////  --- CONTROLLER_DDS --- ///

            ResultReqest ConfigReader::ReadConfigCONTROLLER_DDS(ConfigUnitCP_DDS& conf)
            {
                std::ifstream file;
                std::string help_str;
                ResultReqest res = ResultReqest::OK;
                /// --- open file name_config (config.json) --- ///
                ConfigUnitCP_DDS config;
                config.domen = 0;
                config.type_transmite = TypeTransmite::BroadCast;
                config.ip_base.clear();
                config.ip_reserve.clear();
                config.port_base = 0;
                config.port_reserve = 0;
                config.name_topicanswer.clear();
                config.name_topiccommand.clear();
                config.name_topicanswer = "AnswerOnCommand";
                config.name_topiccommand = "CommandForGates";
                config.type_unit = TypeUnitCP::DDS;

                /// --- parsing str with rapidjson lib --- /// 
                try
                {
                    file.open(name_config, std::ios::in);
                    if (!file.is_open())
                    {
                        throw 1;
                    }

                    /// --- reading the whole file --- ///
                    std::string str((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

                    rapidjson::Document document;
                    document.Parse(str.c_str());

                    if (!document.HasMember("CONTROLLER_DDS")) throw 2;
                    if (!document["CONTROLLER_DDS"].IsObject()) throw 3;

                    if (take_controllerdds_domen(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (take_controllerdds_typetransmite(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (config.type_transmite != TypeTransmite::BroadCast)
                    {
                        if (take_controllerdds_ipbase(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                        if (take_controllerdds_ipreserve(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                        if (take_controllerdds_portbase(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                        if (take_controllerdds_portreserve(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                        if (take_controllerdds_topiccommand(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                        if (take_controllerdds_topicanswer(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    };                           

                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigCONRTOLLER_TCP: error", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigCONRTOLLER_TCP: error", 0, 0);
                    res = ResultReqest::ERR;
                }

                file.close();
                conf = config;
                return res;
            }

            ResultReqest  ConfigReader::take_controllerdds_domen(rapidjson::Document& doc, ConfigUnitCP_DDS& conf)
            {
                ResultReqest res = ResultReqest::OK;

                try
                {
                    if (!doc["CONTROLLER_DDS"].HasMember("Domen")) throw 1;
                    if (!doc["CONTROLLER_DDS"]["Domen"].IsUint()) throw 2;
                    conf.domen = doc["CONTROLLER_DDS"]["Domen"].GetUint();
                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigCONTROLLER_DDS: error Domen", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigCONTROLLER_DDS: error Domen", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_controllerdds_typetransmite(rapidjson::Document& doc, ConfigUnitCP_DDS& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["CONTROLLER_DDS"].HasMember("TypeTransmite")) throw 1;
                    if (!doc["CONTROLLER_DDS"]["TypeTransmite"].IsString()) throw 2;
                    helpstr.clear();
                    helpstr = doc["CONTROLLER_DDS"]["TypeTransmite"].GetString();
                    if (StringToTypeTransmite(helpstr, conf.type_transmite) != ResultReqest::OK) throw 3;
                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigCONTROLLER_DDS: error TypeTratsmiter", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigCONTROLLER_DDS: error TypeTratsmiter", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_controllerdds_ipbase(rapidjson::Document& doc, ConfigUnitCP_DDS& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["CONTROLLER_DDS"].HasMember("IPBase")) throw 1;
                    if (!doc["CONTROLLER_DDS"]["IPBase"].IsString()) throw 2;
                    helpstr = doc["CONTROLLER_DDS"]["IPBase"].GetString();
                    if (CheckIP(helpstr) == ResultReqest::OK)
                    {
                        conf.ip_base.clear();
                        conf.ip_base = helpstr;
                    }
                    else
                    {
                        throw 3;
                    }


                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigContreller_DDS: error IPBase", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigContreller_DDS: error IPBase", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_controllerdds_ipreserve(rapidjson::Document& doc, ConfigUnitCP_DDS& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["CONTROLLER_DDS"].HasMember("IPReserve")) throw 1;
                    if (!doc["CONTROLLER_DDS"]["IPReserve"].IsString()) throw 2;
                    helpstr = doc["CONTROLLER_DDS"]["IPReserve"].GetString();
                    if (CheckIP(helpstr) == ResultReqest::OK)
                    {
                        conf.ip_reserve.clear();
                        conf.ip_reserve = helpstr;
                    }
                    else
                    {
                        throw 3;
                    }


                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigContreller_DDS: error IPReserve", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigContreller_DDS: error IPReserve", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_controllerdds_portbase(rapidjson::Document& doc, ConfigUnitCP_DDS& conf)
            {
                ResultReqest res = ResultReqest::OK;
                unsigned int port;

                try
                {
                    if (!doc["CONTROLLER_DDS"].HasMember("PortBase")) throw 1;
                    if (!doc["CONTROLLER_DDS"]["PortBase"].IsUint()) throw 2;
                    port = doc["CONTROLLER_DDS"]["PortBase"].GetUint();
                    if (port > 65535) throw 3;
                    conf.port_base = port;

                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigController_DDS: error PortBase", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigController_DDS: error PortBase", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_controllerdds_portreserve(rapidjson::Document& doc, ConfigUnitCP_DDS& conf)
            {
                ResultReqest res = ResultReqest::OK;
                unsigned int port;

                try
                {
                    if (!doc["CONTROLLER_DDS"].HasMember("PortReserve")) throw 1;
                    if (!doc["CONTROLLER_DDS"]["PortReserve"].IsUint()) throw 2;
                    port = doc["CONTROLLER_DDS"]["PortReserve"].GetUint();
                    if (port > 65535) throw 3;
                    conf.port_reserve = port;

                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigController_DDS: error PortReserve", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigController_DDS: error PortReserve", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_controllerdds_topiccommand(rapidjson::Document& doc, ConfigUnitCP_DDS& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["CONTROLLER_DDS"].HasMember("TopicCommand")) throw 1;
                    if (!doc["CONTROLLER_DDS"]["TopicCommand"].IsString()) throw 2;
                    helpstr = doc["CONTROLLER_DDS"]["TopicCommand"].GetString();
                    if (helpstr.empty()) throw 3;
                    conf.name_topiccommand = helpstr;
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigControllerDDS: error TopicCommand", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigControllerDDS: error TopicCommand", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_controllerdds_topicanswer(rapidjson::Document& doc, ConfigUnitCP_DDS& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["CONTROLLER_DDS"].HasMember("TopicAnswer")) throw 1;
                    if (!doc["CONTROLLER_DDS"]["TopicAnswer"].IsString()) throw 2;
                    helpstr = doc["CONTROLLER_DDS"]["TopicAnswer"].GetString();
                    if (helpstr.empty()) throw 3;
                    conf.name_topicanswer = helpstr;
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigControllerDDS: error TopicAnswer", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigControllerDDS: error TopicAnswer", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////
            ////////  --- MODULE_IO --- ////////////////////

            ResultReqest  ConfigReader::ReadConfigMODULE_IO(ConfigModule_IO& conf)
            {
                std::ifstream file;
                std::string help_str;
                ResultReqest res = ResultReqest::OK;
                /// --- open file name_config (config.json) --- ///
                ConfigModule_IO config;
                config.domen = 0;
                config.type_transmiter = TypeTransmiter::Broadcast;
                config.ip_base.clear();
                config.ip_reserve.clear();
                config.port_base = 0;
                config.port_reserve = 0;
                config.topic_info.clear();
                config.topic_info = "TopicInfoUnits";

                /// --- parsing str with rapidjson lib --- /// 
                try
                {
                    file.open(name_config, std::ios::in);
                    if (!file.is_open())
                    {
                        throw 1;
                    }

                    /// --- reading the whole file --- ///
                    std::string str((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

                    rapidjson::Document document;
                    document.Parse(str.c_str());

                    if (!document.HasMember("MODULE_IO")) throw 2;
                    if (!document["MODULE_IO"].IsObject()) throw 3;

                    if (take_moduleio_domen(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (take_moduleio_typetransmite(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    if (config.type_transmiter != TypeTransmiter::Broadcast)
                    {
                        if (take_moduleio_ipbase(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                        if (take_moduleio_ipreserve(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                        if (take_moduleio_portbase(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                        if (take_moduleio_portreserve(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    }
                    if (take_moduleio_topicinfo(document, config) == ResultReqest::ERR) res = ResultReqest::ERR;
                    
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigMODULE_IO: ", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigMODULE_IO: ", 0, 0);
                    res = ResultReqest::ERR;
                }

                file.close();
                conf = config;
                return res;
            }

            ResultReqest  ConfigReader::take_moduleio_domen(rapidjson::Document& doc, ConfigModule_IO& conf)
            {
                ResultReqest res = ResultReqest::OK;

                try
                {
                    if (!doc["MODULE_IO"].HasMember("Domen")) throw 1;
                    if (!doc["MODULE_IO"]["Domen"].IsUint()) throw 2;
                    conf.domen = doc["MODULE_IO"]["Domen"].GetUint();
                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigMODULE_IO: error Domen", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigMODULE_IO: error Domen", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_moduleio_typetransmite(rapidjson::Document& doc, ConfigModule_IO& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["MODULE_IO"].HasMember("TypeTransmite")) throw 1;
                    if (!doc["MODULE_IO"]["TypeTransmite"].IsString()) throw 2;
                    helpstr.clear();
                    helpstr = doc["MODULE_IO"]["TypeTransmite"].GetString();
                    if (StringToTypeTransmiter(helpstr, conf.type_transmiter) != ResultReqest::OK) throw 3;
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

            ResultReqest  ConfigReader::take_moduleio_ipbase(rapidjson::Document& doc, ConfigModule_IO& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["MODULE_IO"].HasMember("IPBase")) throw 1;
                    if (!doc["MODULE_IO"]["IPBase"].IsString()) throw 2;
                    helpstr = doc["MODULE_IO"]["IPBase"].GetString();
                    if (CheckIP(helpstr) == ResultReqest::OK)
                    {
                        conf.ip_base.clear();
                        conf.ip_base = helpstr;
                    }
                    else
                    {
                        throw 3;
                    }


                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigMODULE_IO: error IPBase", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigMODULE_IO: error IPBase", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_moduleio_ipreserve(rapidjson::Document& doc, ConfigModule_IO& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["MODULE_IO"].HasMember("IPReserve")) throw 1;
                    if (!doc["MODULE_IO"]["IPReserve"].IsString()) throw 2;
                    helpstr = doc["MODULE_IO"]["IPReserve"].GetString();
                    if (CheckIP(helpstr) == ResultReqest::OK)
                    {
                        conf.ip_reserve.clear();
                        conf.ip_reserve = helpstr;
                    }
                    else
                    {
                        throw 3;
                    }


                }
                catch (int& e)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigMODULE_IO: error IPReserve", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogWARNING("Error ConfigReader: error ReaderConfigMODULE_IO: error IPReserve", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_moduleio_portbase(rapidjson::Document& doc, ConfigModule_IO& conf)
            {
                ResultReqest res = ResultReqest::OK;
                unsigned int port;

                try
                {
                    if (!doc["MODULE_IO"].HasMember("PortBase")) throw 1;
                    if (!doc["MODULE_IO"]["PortBase"].IsUint()) throw 2;
                    port = doc["MODULE_IO"]["PortBase"].GetUint();
                    if (port > 65535) throw 3;
                    conf.port_base = port;

                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigMODULE_IO: error PortBase", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigMODULE_IO: error PortBase", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_moduleio_portreserve(rapidjson::Document& doc, ConfigModule_IO& conf)
            {
                ResultReqest res = ResultReqest::OK;
                unsigned int port;

                try
                {
                    if (!doc["MODULE_IO"].HasMember("PortReserve")) throw 1;
                    if (!doc["MODULE_IO"]["PortReserve"].IsUint()) throw 2;
                    port = doc["MODULE_IO"]["PortReserve"].GetUint();
                    if (port > 65535) throw 3;
                    conf.port_reserve = port;

                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigMODULE_IO: error PortReserve", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigMODULE_IO: error PortReserve", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }

            ResultReqest  ConfigReader::take_moduleio_topicinfo(rapidjson::Document& doc, ConfigModule_IO& conf)
            {
                ResultReqest res = ResultReqest::OK;
                std::string helpstr;

                try
                {
                    if (!doc["MODULE_IO"].HasMember("TopicInfo")) throw 1;
                    if (!doc["MODULE_IO"]["TopicInfo"].IsString()) throw 2;
                    helpstr = doc["MODULE_IO"]["TopicInfo"].GetString();
                    if (helpstr.empty()) throw 3;
                    conf.topic_info = helpstr;
                }
                catch (int& e)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigMODULE_IO: error TopicInfo", e, 0);
                    res = ResultReqest::ERR;
                }
                catch (...)
                {
                    log->WriteLogERR("Error ConfigReader: error ReaderConfigMODULE_IO: error TopicInfo", 0, 0);
                    res = ResultReqest::ERR;
                }

                return res;
            }



            ////////////////////////////////////////////////////////////////////////////////////////////////

            void ConfigReader::SetNameConfigFile(std::string str)
            {
                name_config.clear();
                name_config = str;
            }

           /* ResultReqest ReadConfigTransferUnits(std::vector<ConfigDDSUnit>& vector_result)
            {
                unsigned int Domen;
                std::ifstream file;
                file.open(name_configunits, std::ios::in);
                if (!file.is_open())
                {
                    log->WriteLogERR("ERROR READ CONFIGUNITS FILE", 0, 0);
                    return ResultReqest::ERR;
                }

                std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                vector_result.clear();

                try
                {
                    rapidjson::Document document;
                    document.Parse(str.c_str());

                    if (document["Units"].IsArray())
                    {
                        for (rapidjson::SizeType i = 0; i < document["Units"].Size(); i++)
                        {
                            {
                                ConfigDDSUnit unit;
                                std::string helpstr;
                                unit.Domen = document["Units"][i]["Domen"].GetUint();

                                helpstr = document["Units"][i]["TypeUnit"].GetString();
                                unit.TypeUnit = helpstr == "Publisher" ? TypeDDSUnit::PUBLISHER :
                                    helpstr == "Subscriber" ? TypeDDSUnit::SUBSCRIBER : TypeDDSUnit::Empty;

                                helpstr.clear();
                                helpstr = document["Units"][i]["TypeTransmite"].GetString();
                                unit.Transmiter = helpstr.compare("TCP") == 0 ? TypeTransmiter::TCP :
                                    helpstr.compare("UDP") == 0 ? TypeTransmiter::UDP : TypeTransmiter::Broadcast;

                                helpstr.clear();
                                helpstr = document["Units"][i]["TypeAdapter"].GetString();
                                unit.Adapter = helpstr.compare("SharedMemory") == 0 ? TypeAdapter::SharedMemory :
                                    helpstr.compare("DDS") == 0 ? TypeAdapter::DDS :
                                    helpstr.compare("DTS") == 0 ? TypeAdapter::DTS :
                                    helpstr.compare("OPC_UA") == 0 ? TypeAdapter::OPC_UA :
                                    helpstr.compare("SMTP") == 0 ? TypeAdapter::SMTP : TypeAdapter::Null;

                                unit.PointName = document["Units"][i]["PointName"].GetString();

                                helpstr.clear();
                                helpstr = document["Units"][i]["TypeData"].GetString();
                                unit.Typedata = helpstr == "Analog" ? TypeData::ANALOG : helpstr == "Discrete" ? TypeData::DISCRETE : helpstr == "Binar" ? TypeData::BINAR : TypeData::ZERO;
                                unit.Size = document["Units"][i]["Size"].GetUint();
                                unit.Frequency = document["Units"][i]["Frequency"].GetUint();

                                if (unit.Transmiter != TypeTransmiter::Broadcast)
                                {
                                    unit.IP_MAIN = document["Units"][i]["IP_Main"].GetString();
                                    unit.Port_MAIN = document["Units"][i]["Port_Main"].GetUint();
                                    unit.IP_RESERVE = document["Units"][i]["IP_Reserve"].GetString();
                                    unit.Port_RESERVE = document["Units"][i]["Port_Reserve"].GetUint();
                                }
                                else
                                {
                                    unit.IP_MAIN.clear();
                                    unit.Port_MAIN = 0;
                                    unit.IP_RESERVE.clear();
                                    unit.Port_RESERVE = 0;
                                }
                                vector_result.push_back(unit);
                            }
                        }
                    }
                    else
                    {
                        log->WriteLogERR("ERROR READ CONFIGUNITS FILE", 1, 0);
                    }
                }
                catch (...)
                {
                    log->WriteLogERR("ERROR READ CONFIGUNITS FILE", 2, 0);
                    return ResultReqest::ERR;
                }

                file.close();
                return ResultReqest::OK;
            };
            */

            ResultReqest ConfigReader::StringToLogMode(std::string str, LoggerSpace::LogMode& value)
            {
                ResultReqest res = ResultReqest::ERR;

                if (str.compare("DEBUG") == 0)
                {
                    value = LoggerSpace::LogMode::DEBUG;
                    return ResultReqest::OK;
                }

                if (str.compare("INFO") == 0)
                {
                    value = LoggerSpace::LogMode::INFO;
                    return ResultReqest::OK;
                }

                if (str.compare("WARNING") == 0)
                {
                    value = LoggerSpace::LogMode::WARNING;
                    return ResultReqest::OK;
                }

                if (str.compare("ERR") == 0)
                {
                    value = LoggerSpace::LogMode::ERR;
                    return ResultReqest::OK;
                }


                return res;
            };

            ResultReqest ConfigReader::StringToLogStatus(std::string str, LoggerSpace::Status& value)
            {
                ResultReqest res = ResultReqest::ERR;

                if (str.compare("ON") == 0)
                {
                    value = LoggerSpace::Status::ON;
                    return ResultReqest::OK;
                }

                if (str.compare("OFF") == 0)
                {
                    value = LoggerSpace::Status::OFF;
                    return ResultReqest::OK;
                }

                return res;
            };

            ResultReqest ConfigReader::StringToTypeTransmiter(std::string str, TypeTransmiter& value)
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

            ResultReqest ConfigReader::StringToTypeTransmite(std::string str, TypeTransmite& value)
            {

                if (str.compare("Broadcast") == 0)
                {
                    value = TypeTransmite::BroadCast;
                    return ResultReqest::OK;
                }

                if (str.compare("TCP") == 0)
                {
                    value = TypeTransmite::TCP;
                    return ResultReqest::OK;
                }

                if (str.compare("UDP") == 0)
                {
                    value = TypeTransmite::UDP;
                    return ResultReqest::OK;
                }

                return ResultReqest::ERR;
            };

            ResultReqest ConfigReader::CheckIP(std::string str)
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
        };
    };
};




