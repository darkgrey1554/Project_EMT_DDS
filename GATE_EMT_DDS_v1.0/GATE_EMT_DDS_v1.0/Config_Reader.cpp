#include "Config_Reader.h"

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

ResultReqest ConfigReader::ReadConfigGATE(ConfigGate& conf)
{

    std::ifstream file;
    std::string help_str;
    ResultReqest res = ResultReqest::OK;
    /// --- open file name_config (default = config.json) --- ////
    file.open(name_config, std::ios::in);
    if (!file.is_open())
    {
        log->WriteLogERR("ERROR READ CONFIG FILE", 0, 0);
        res = ResultReqest::ERR;
        return res;
    }

    /// --- reading the whole file --- ///
    std::string str((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());   
    
    /// --- parsing configfile with rapidjson lib --- ///
    try 
    {
        rapidjson::Document document;
        document.Parse(str.c_str());

        if (document["GATE"].IsObject())
        {
            conf.IdGate = document["GATE"]["IdGate"].GetUint();
            conf.Domen = document["GATE"]["Domen"].GetUint();
            conf.TopicSubscritionCommand = document["GATE"]["TopicSubscritionCommand"].GetString();
            conf.TopicPublicationAnswer = document["GATE"]["TopicPublicationAnswer"].GetString();
            conf.TopicSubscribtionInfoConfig = document["GATE"]["TopicSubscribtionInfoConfig"].GetString();

            help_str = document["GATE"]["TypeTransmite"].GetString();
            conf.TypeTransmite = help_str.compare("TCP") == 0 ? TypeTransmiter::TCP :
                help_str.compare("UDP") ? TypeTransmiter::UDP : TypeTransmiter::Broadcast;

            if (conf.TypeTransmite != TypeTransmiter::Broadcast)
            {
                conf.IPSubscribtion = document["GATE"]["IPSubscribtion"].GetString();
                conf.PortSubscribtion = document["GATE"]["PortSubscribtion"].GetUint();
                conf.IPPublication = document["GATE"]["IPPublication"].GetString();
                conf.PortPublication = document["GATE"]["PortPublication"].GetUint();
            }
        }
        else
        {
            log->WriteLogERR("ERROR READ CONFIG GATE", 1, 0);
            res = ResultReqest::ERR;
        }
    }
    catch(...)
    {
        log->WriteLogERR("ERROR READ CONFIG GATE", 2, 0);
        res = ResultReqest::ERR;
    }
   
    file.close();
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

    /// --- open file name_config (config.json) --- ///
    file.open(name_config, std::ios::in);
    if (!file.is_open())
    {
        log->WriteLogERR("ERROR READ CONFIG FILE", 1, 0);
        res = ResultReqest::ERR;
        return res;
    }

    /// --- reading the whole file --- ///
    std::string str((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    /// --- parsing str with rapidjson lib --- /// 
    try
    {
        rapidjson::Document document;
        document.Parse(str.c_str());

        if (document["LOGGER"].IsObject())
        {
            conf.LogName = document["LOGGER"]["LogName"].GetString();
            conf.SysLogName = document["LOGGER"]["SysLogName"].GetString();

            help_str = document["LOGGER"]["LogMode"].GetString();
            conf.LogMode = help_str.compare("Info") == 0 ? LoggerSpace::LogMode::INFO :
                help_str.compare("Warning") == 0 ? LoggerSpace::LogMode::WARNING :
                help_str.compare("Error") == 0 ? LoggerSpace::LogMode::ERR : LoggerSpace::LogMode::DEBUG;

            help_str.clear();
            help_str = document["LOGGER"]["StatusLog"].GetString();
            conf.StatusLog = help_str.compare("TURN") == 0 ? LoggerSpace::Status::ON : LoggerSpace::Status::OFF;

            help_str.clear();
            help_str = document["LOGGER"]["StatusSysLog"].GetString();
            conf.StatusSysLog = help_str.compare("TURN") == 0 ? LoggerSpace::Status::ON : LoggerSpace::Status::OFF;
            conf.SizeLogFile = document["LOGGER"]["SizeLogFile"].GetUint();
        }
        else
        {
            log->WriteLogERR("ERROR READ CONFIG LOGGER", 0, 0);
            res = ResultReqest::ERR;
        }        
    }
    catch (...)
    {
        log->WriteLogERR("ERROR READ CONFIG LOGGER", 1, 0);
        res = ResultReqest::ERR;
    }


    file.close();
    return res;
};

///////////////////////////////////////////////////////////////
// read configuration header LOGGER in file config.json
// conf - result read configuration
// return - success of the operation
///////////////////////////////////////////////////////////////

ResultReqest ConfigReader::ReadConfigMANAGER(ConfigManager& conf)
{
    std::ifstream file;
    std::string help_str;
    ResultReqest res = ResultReqest::OK;
    /// --- open file name_config (config.json) --- ///
    file.open(name_config, std::ios::in);
    if (!file.is_open())
    {
        log->WriteLogERR("ERROR READ CONFIG FILE", 0, 0);
        res = ResultReqest::ERR;
        return res;
    }

    /// --- reading the whole file --- ///
    std::string str((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    /// --- parsing str with rapidjson lib --- /// 
    try
    {
        rapidjson::Document document;
        document.Parse(str.c_str());

        /// --- check object Manager and read configuration --- ///
        if (document["MANAGER"].IsObject())
        {
            conf.IP = document["MANAGER"]["IP"].GetString();
            conf.Port = document["MANAGER"]["Port"].GetUint();
        }
        else
        {
            log->WriteLogERR("ERROR READ CONFIG MANAGER", 1, 0);
            res = ResultReqest::ERR;
        }

    }
    catch (...)
    {
        log->WriteLogERR("ERROR READ CONFIG MANAGER", 2, 0);
        res = ResultReqest::ERR;
    }

    file.close();
    return res;
}


ResultReqest ConfigReader::ReadConfigTransferUnits(std::vector<ConfigDDSUnit>& vector_result)
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


ResultReqest ConfigReader::WriteConfigFile()
{
    return ResultReqest::ERR;
}


/*ResultReqest ConfigReaderDDS::GetResult(std::vector<ConfigDDSUnit>& vector_result)
{
    return ResultReqest::ERR;
};*/
/*ResultReqest ConfigReaderDDS::SetNameConfigFile(std::string name)
{
    return ResultReqest::ERR;
};*/

/*ResultReqest ConfigReaderDDS::WriteConfigFile() 
{
    return ResultReqest::ERR;
};*/

/*ResultReqest ConfigReaderDDS::ReadConfigDDS_Subscribers(std::string file_name)
{
    FILE* config_file = NULL;
    ResultReqest result = ResultReqest::OK;
    char simvol = 0;
    std::string str_info;
    std::string helpstr;
    int res_read = 0;
    int pos[2] = { 0,0 };
    int count = 0;
    char status = 0;
    ConfigDDSUnit subscriber;
    
    config_file = fopen(file_name.c_str(), "r");
    if (config_file == NULL)
    {
        result = ResultReqest::ERR;
        log->WriteLogERR("ERROR READ CONFIG DDS", 1, 0);
        return result ;
    }

    for (;;)
    {
        simvol = 0;
        str_info.clear();
        while (simvol != '\n' && res_read != EOF)
        {
            res_read = fscanf(config_file, "%c", &simvol);
            if ((simvol > 0x1F || simvol == '\t') && res_read != EOF) str_info += simvol;
        }

        if (res_read == EOF ) //&& str_info.find("[END]") == -1)
        {
            if (SubscribersDDS.empty()) 
            {
                log->WriteLogERR("ERROR READ CONFIG DDS", 2, 0);
                result = ResultReqest::ERR;
            }
            break;
        }

        if (str_info.substr(0, 16) == "[SUBSCRIBER]")
        {
            status = 1;
            subscriber.clear();
            continue;
        }

        if (status == 1)
        {
            pos[0] = str_info.find('\t', 0);
            /// чтение IP адреса
            if (str_info.find("IP_Address_MAIN") != -1)
            {
                if (pos[0] == -1) 
                { 
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 3, 0);
                    result = ResultReqest::ERR;
                    continue; 
                }                
                subscriber.IP_MAIN = str_info.substr((size_t)pos[0] + 1);
                continue;
            }
            if (str_info.find("IP_Address_RESERVE") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 4, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                subscriber.IP_RESERVE = str_info.substr((size_t)pos[0] + 1);
                continue;
            }
            /// чтение порта
            if (str_info.find("Port_MAIN") != -1)
            {
                if (pos[0] == -1) 
                { 
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 5, 0);
                    result = ResultReqest::ERR;
                    continue; 
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) == 0 || strtol(helpstr.c_str(), NULL, 10) > 65535) 
                { 
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 6, 0);
                    continue; 
                }
                subscriber.Port_MAIN = atoi(helpstr.c_str());
                continue;
            }
            if (str_info.find("Port_RESERVE") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 7, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) == 0 || strtol(helpstr.c_str(), NULL, 10) > 65535)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 8, 0);
                    continue;
                }
                subscriber.Port_RESERVE = atoi(helpstr.c_str());
                continue;
            }
            /// чтение домена
            if (str_info.find("Domen") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 9, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) < 0 || strtol(helpstr.c_str(), NULL, 10) > 230) 
                { 
                    log->WriteLogWARNING("ERROR READ CONFIG", 10, 0);
                    result = ResultReqest::ERR;
                    continue; 
                }
                subscriber.Domen = atoi(helpstr.c_str());
                continue;
            }

            //чтение имени листа KKS
            if (str_info.find("NameListKKSIn") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 11, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                subscriber.NameListKKS = str_info.substr((size_t)pos[0] + 1);
                continue;
            }

            //чтение имени SharedMemory
            if (str_info.find("NameSharedMemory") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 12, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                subscriber.NameMemory = str_info.substr((size_t)pos[0] + 1);
                continue;
            }

            if (str_info.find("[END]") != -1)
            {
                SubscribersDDS.push_back(subscriber);
                status = 0;
                continue;
            }
        }
    }
    fclose(config_file);
    return result;
}*/

/*ResultReqest ConfigReaderDDS::ReadConfigDDS_Publishers(std::string file_name)
{
    FILE* config_file = NULL;
    ResultReqest result = ResultReqest::OK;
    char simvol = 0;
    std::string str_info;
    std::string helpstr;
    int res_read = 0;
    int pos[2] = { 0,0 };
    int count = 0;
    char status = 0;
    ConfigDDSUnit publisher;

    config_file = fopen(file_name.c_str(), "r");
    if (config_file == NULL)
    {
        result = ResultReqest::ERR;
        log->WriteLogERR("ERROR READ CONFIG DDS", 1, 0);
        return result;
    }

    for (;;)
    {
        simvol = 0;
        str_info.clear();
        while (simvol != '\n' && res_read != EOF)
        {
            res_read = fscanf(config_file, "%c", &simvol);
            if ((simvol > 0x1F || simvol == '\t') && res_read != EOF) str_info += simvol;
        }

        if (res_read == EOF) //&& str_info.find("[END]") == -1)
        {
            if (PublishersDDS.empty())
            {
                log->WriteLogERR("ERROR READ CONFIG DDS", 2, 0);
                result = ResultReqest::ERR;
            }
            break;
        }

        if (str_info.substr(0, 16) == "[PUBLISHER]")
        {
            status = 1;
            publisher.clear();
            continue;
        }

        if (status == 1)
        {

            pos[0] = str_info.find('\t', 0);
            /// чтение IP адреса
            if (str_info.find("IP_Address_MAIN") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 3, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                publisher.IP_MAIN = str_info.substr((size_t)pos[0] + 1);
                continue;
            }
            if (str_info.find("IP_Address_RESERVE") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 4, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                publisher.IP_RESERVE = str_info.substr((size_t)pos[0] + 1);
                continue;
            }
            /// чтение порта
            if (str_info.find("Port_MAIN") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 5, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) == 0 || strtol(helpstr.c_str(), NULL, 10) > 65535)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 6, 0);
                    continue;
                }
                publisher.Port_MAIN = atoi(helpstr.c_str());
                continue;
            }
            if (str_info.find("Port_RESERVE") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 7, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) == 0 || strtol(helpstr.c_str(), NULL, 10) > 65535)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 8, 0);
                    continue;
                }
                publisher.Port_RESERVE = atoi(helpstr.c_str());
                continue;
            }
            /// чтение домена
            if (str_info.find("Domen") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 9, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) < 0 || strtol(helpstr.c_str(), NULL, 10) > 230)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG", 10, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                publisher.Domen = atoi(helpstr.c_str());
                continue;
            }

            if (str_info.find("NameListKKSOut") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 11, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                publisher.NameListKKS = str_info.substr((size_t)pos[0] + 1);
                continue;
            }

            if (str_info.find("NameSharedMemory") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 12, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                publisher.NameMemory = str_info.substr((size_t)pos[0] + 1);
                continue;
            }

            if (str_info.find("[END]") != -1)
            {
                PublishersDDS.push_back(publisher);
                status = 0;
                continue;
            }
        }
    }
    fclose(config_file);
    return result;
}*/

/*ResultReqest ConfigReaderDDS::ReadConfigDDS(std::string file_name)
{
    ResultReqest res = ResultReqest::OK;
    res=this->ReadConfigDDS_Publishers(file_name);
    res=this->ReadConfigDDS_Subscribers(file_name);
    return res;
};*/

