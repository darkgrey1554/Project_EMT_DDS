#include "Config_Reader.h"

ResultReqest ConfigReaderDDS::ReadConfigDDS_Subscribers(std::string file_name)
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
    ConfigSubscriber subscriber;
    
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

        if (str_info.substr(0, 16) == "[DDS_SUBSCRIBER]")
        {
            status = 1;
            subscriber.clear();
            continue;
        }

        if (status == 1)
        {
            
            pos[0] = str_info.find('\t', 0);
            /// чтение IP адреса
            if (str_info.find("IP_Address") != -1)
            {
                if (pos[0] == -1) 
                { 
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 3, 0);
                    result = ResultReqest::ERR;
                    continue; 
                }                
                subscriber.IP = str_info.substr((size_t)pos[0] + 1);
                continue;
            }
            /// чтение порта
            if (str_info.find("Port") != -1)
            {
                if (pos[0] == -1) 
                { 
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 4, 0);
                    result = ResultReqest::ERR;
                    continue; 
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) == 0 || strtol(helpstr.c_str(), NULL, 10) > 65535) 
                { 
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 11, 0);
                    continue; 
                }
                subscriber.Port = atoi(helpstr.c_str());
                continue;
            }
            /// чтение домена
            if (str_info.find("Domen") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 5, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) < 0 || strtol(helpstr.c_str(), NULL, 10) > 230) 
                { 
                    log->WriteLogWARNING("ERROR READ CONFIG", 6, 0);
                    result = ResultReqest::ERR;
                    continue; 
                }
                subscriber.Domen = atoi(helpstr.c_str());
                continue;
            }

            if (str_info.find("TypeUnit") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 7, 0);
                    continue;
                }
                if (str_info.substr((size_t)pos[0] + 1) == "MAIN") 
                { 
                    subscriber.TypeUnit = TypeUnitTransport::MAIN; 
                    continue; 
                }
                if (str_info.substr((size_t)pos[0] + 1) == "RESERVER") 
                { 
                    subscriber.TypeUnit = TypeUnitTransport::RESERVER; 
                    continue; 
                }
                log->WriteLogWARNING("ERROR READ CONFIG DDS", 8, 0);
                result = ResultReqest::ERR;
                continue;
            }

            if (str_info.find("NameListKKSIn") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 9, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                subscriber.NameListKKSIn = str_info.substr((size_t)pos[0] + 1);
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
}

ResultReqest ConfigReaderDDS::ReadConfigDDS_Publishers(std::string file_name)
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
    ConfigPublisher publisher;

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

        if (str_info.substr(0, 16) == "[DDS_PUBLISHER]")
        {
            status = 1;
            publisher.clear();
            continue;
        }

        if (status == 1)
        {

            pos[0] = str_info.find('\t', 0);
            /// чтение IP адреса
            if (str_info.find("IP_Address") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 3, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                publisher.IP = str_info.substr((size_t)pos[0] + 1);
                continue;
            }
            /// чтение порта
            if (str_info.find("Port") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 4, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) == 0 || strtol(helpstr.c_str(), NULL, 10) > 65535)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 11, 0);
                    continue;
                }
                publisher.Port = atoi(helpstr.c_str());
                continue;
            }
            /// чтение домена
            if (str_info.find("Domen") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 5, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                helpstr.clear();
                helpstr = str_info.substr((size_t)pos[0] + 1);
                if (strtol(helpstr.c_str(), NULL, 10) < 0 || strtol(helpstr.c_str(), NULL, 10) > 230)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG", 6, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                publisher.Domen = atoi(helpstr.c_str());
                continue;
            }

            if (str_info.find("TypeUnit") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 7, 0);
                    continue;
                }
                if (str_info.substr((size_t)pos[0] + 1) == "MAIN")
                {
                    publisher.TypeUnit = TypeUnitTransport::MAIN;
                    continue;
                }
                if (str_info.substr((size_t)pos[0] + 1) == "RESERVER")
                {
                    publisher.TypeUnit = TypeUnitTransport::RESERVER;
                    continue;
                }
                log->WriteLogWARNING("ERROR READ CONFIG DDS", 8, 0);
                result = ResultReqest::ERR;
                continue;
            }

            if (str_info.find("NameListKKSOut") != -1)
            {
                if (pos[0] == -1)
                {
                    log->WriteLogWARNING("ERROR READ CONFIG DDS", 9, 0);
                    result = ResultReqest::ERR;
                    continue;
                }
                publisher.NameListKKSOut = str_info.substr((size_t)pos[0] + 1);
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
}

ResultReqest ConfigReaderDDS::ReadConfigDDS(std::string file_name)
{
    ResultReqest res = ResultReqest::OK;
    res=this->ReadConfigDDS_Publishers(file_name);
    res=this->ReadConfigDDS_Subscribers(file_name);
    return res;
};