#include "Config_Reader.h"
#include <rapidjson/document.h>
#include <ifstream>



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


ResultReqest ConfigReaderDDS::ReadConfigFile()
{
    
    
    
    ResultReqest result = ResultReqest::ERR;
    return result;
}