#include "Config_Reader.h"


/*ConfigReader<ConfigDDSUnit>* CreateConfigReader(Type_Gate type)
{
    ConfigReader<ConfigDDSUnit>* res = new ConfigReaderDDS();
    return  res;
};*/

ConfigReaderDDS::ConfigReaderDDS()
{
    log = LoggerSpace::Logger::getpointcontact();
}

ResultReqest ConfigReaderDDS::ReadConfigGate(ConfigGate& conf)
{
    unsigned int Domen;
    std::ifstream file;
    file.open(name_config, std::ios::in);
    if (!file.is_open())
    {
        log->WriteLogERR("ERROR READ CONFIG FILE", 1, 0);
        return ResultReqest::ERR;
    }

    std::string str((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());   
    
    try 
    {
        rapidjson::Document document;
        document.Parse(str.c_str());
        config.IdGate = document["IdGate"].GetUint();
        config.Domen = document["Domen"].GetUint();
        config.TypeTransmite = document["TypeTransmite"].GetString();;
        config.IPSubscribtion = document["IPSubscribtion"].GetString();
        config.PortSubscribtion = document["PortSubscribtion"].GetUint();
        config.TopicSubscritionCommand = document["TopicSubscritionCommand"].GetString();
        config.TopicSubscribtionInfoConfig = document["TopicSubscribtionInfoConfig"].GetString();
        config.IPPublication = document["IPPublication"].GetString();
        config.PortPublication = document["PortPublication"].GetUint();
        config.TopicPublicationAnswer = document["TopicPublicationAnswer"].GetString();
    }
    catch(...)
    {
        log->WriteLogERR("ERROR READ CONFIG FILE", 2, 0);
        file.close();
        return ResultReqest::ERR;
    }
   
    file.close();
    return ResultReqest::OK;
}

ResultReqest ConfigReaderDDS::ReadConfigTransferUnits(std::vector<ConfigDDSUnit>& vector_result)
{
    unsigned int Domen;
    std::ifstream file;
    file.open(name_configunits, std::ios::in);
    if (!file.is_open())
    {
        log->WriteLogERR("ERROR READ CONFIGUNITS FILE", 1, 0);
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
                    unit.TypeUnit = helpstr == "Publisher" ? TypeDDSUnit::PUBLISHER : helpstr == "Subscriber" ? TypeDDSUnit::SUBSCRIBER : TypeDDSUnit::Empty;
                    unit.TopicName = document["Units"][i]["TopicName"].GetString();
                    unit.SMName = document["Units"][i]["SMName"].GetString();
                    helpstr.clear();
                    helpstr = document["Units"][i]["TypeUnit"].GetString();
                    unit.Typedata = helpstr == "Analog" ? TypeData::ANALOG : helpstr == "Discrete" ? TypeData::DISCRETE : helpstr == "Binar" ? TypeData::BINAR : TypeData::ZERO;
                    unit.Size = document["Units"][i]["Size"].GetUint();
                    unit.Frequency = document["Units"][i]["Frequency"].GetUint();
                    unit.IP_MAIN = document["Units"][i]["IP_Main"].GetString();
                    unit.Port_MAIN = document["Units"][i]["Port_Main"].GetUint();
                    unit.IP_RESERVE = document["Units"][i]["IP_Reserve"].GetString();
                    unit.Port_RESERVE = document["Units"][i]["Port_Reserve"].GetUint();
                    vector_result.push_back(unit);
                }
            }
        }
    }
    catch (...)
    {
        log->WriteLogERR("ERROR READ CONFIGUNITS FILE", 2, 0);
        file.close();
        return ResultReqest::ERR;
    }

    file.close();
    return ResultReqest::OK;



    return ResultReqest::ERR;
};

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

