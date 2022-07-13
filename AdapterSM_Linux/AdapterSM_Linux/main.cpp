#include <cstdio>
#include <iostream>
#include "TimeConverter.hpp"
#include "LoggerScada.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include "Adapters.hpp"
#include "AdapterSharedMemory.hpp"

void f_create()
{
    int hnd = shm_open("first_memory", O_CREAT | O_RDWR, 0777);
    if (hnd != -1)
    {
        std::cout << "Create done" << std::endl;
    }

    if (ftruncate(hnd, 10) != -1)
    {
        std::cout << "Memory load " << std::endl;
    }

    char* buf = (char*) mmap(NULL, 10, PROT_READ | PROT_WRITE, MAP_SHARED, hnd, 0);

    if (buf == MAP_FAILED)
    {
        std::cout << "Error mmap" << std::endl;
    }
    else
    {
        std::cout << "mmap done" << std::endl;
    }

    *(int*)buf = 10;
    *((int*)buf + 1) = 20;

    char s;
    std::cin >> s;

    munmap(buf, 10);
    close(hnd);
    shm_unlink("first_memory");

    return;
}

void f_read()
{
    int hnd = shm_open("first_memory", O_RDWR, 0777);

    if (hnd != -1)
    {
        std::cout << "Open read done" << std::endl;
    }
    else
    {
        std::cout << "Error Open read" << std::endl;
        return;
    }

    char* buf = (char*)mmap(NULL, 10, PROT_READ | PROT_WRITE, MAP_SHARED, hnd, 0);

    if (buf == MAP_FAILED)
    {
        std::cout << "Error mmap" << std::endl;
    }
    else
    {
        std::cout << "mmap done" << std::endl;
    }

    std::cout << *(int*)buf << std::endl;
    std::cout << *((int*)buf+1) << std::endl;

    char s;
    std::cin >> s;

    munmap(buf, 10);
    close(hnd);

}

void f_crash()
{
    shm_unlink("first_memory");
}

void target()
{
    LoggerSpaceScada::ConfigLogger confg;
    confg.level = LoggerSpaceScada::LevelLog::Debug;
    LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, confg);

    std::shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> config_out =
        std::make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
    config_out->NameChannel = "channel_1";
    config_out->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
    config_out->size_int_data = 100;
    config_out->vec_tags_source.resize(100);
    int counter = 0;

    for (auto& it : config_out->vec_tags_source)
    {
        it.id_tag = counter;
        it.is_array = false;
        it.offset = counter;
        it.type = scada_ate::gate::adapter::TypeValue::INT;
        it.mask = 0;

        counter++;
    }

    scada_ate::gate::adapter::IAdapter_ptr adapter_in = scada_ate::gate::adapter::CreateAdapter(config_out);
    adapter_in->InitAdapter();
    std::deque<scada_ate::gate::adapter::SetTags>* _data;

    while (1)
    {
        adapter_in->ReadData(&_data);
        counter = 0;
        
        for (auto it : config_out->vec_tags_source)
        {
            std::cout << "[" << it.id_tag << "] = " << _data->begin()->map_int_data[it].value << std::endl;
        }

        std::cout << "////////////" << std::endl << std::endl;

        sleep(2);
    }
    
}

void source()
{
    LoggerSpaceScada::ConfigLogger confg;
    confg.level = LoggerSpaceScada::LevelLog::Debug;
    LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, confg);

    std::shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> config_in =
        std::make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
    config_in->NameChannel = "channel_1";
    config_in->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
    config_in->size_int_data = 100;
    config_in->vec_link_tags.resize(100);
    int counter = 0;

    for (auto& it : config_in->vec_link_tags)
    {
        it.source.id_tag = counter;
        it.source.is_array = false;
        it.source.offset = counter;
        it.source.type = scada_ate::gate::adapter::TypeValue::INT;
        it.source.mask = 0;

        it.target.id_tag = counter;
        it.target.is_array = false;
        it.target.offset = counter;
        it.target.type = scada_ate::gate::adapter::TypeValue::INT;
        it.target.mask = 0;

        counter++;
    }

    std::vector<scada_ate::gate::adapter::InfoTag> tags(100);
    std::deque<scada_ate::gate::adapter::SetTags> _data;
    _data.resize(1);
    auto& _map = _data.begin()->map_int_data;
    counter = 0;
    for (auto& it : tags)
    {
        it.id_tag = counter;
        it.is_array = false;
        it.offset = counter;
        it.type = scada_ate::gate::adapter::TypeValue::INT;
        it.mask = 0;

        _map[it] = { 0, counter , 1 };
        counter++;
    }

    scada_ate::gate::adapter::IAdapter_ptr adapter_in = scada_ate::gate::adapter::CreateAdapter(config_in);
    adapter_in->InitAdapter();


    while (1)
    {
        for (auto it = _map.begin(); it != _map.end(); it++)
        {
            it->second.value++;
        }

        adapter_in->WriteData(_data);
        sleep(2);
    }

}


int main(int ar, char** argv)
{
    if (std::strcmp("source", argv[1]) == 0)
    {
        source();
    }
    else if (std::strcmp("target", argv[1]) == 0)
    {
        target();
    }

    return 0;
}

//int main()
//{
//    std::shared_ptr<LoggerSpaceScada::ILoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
//
//    log->Info("dasdasd , {}", 10);
//
//    sem_t* sem = sem_open("test_sem", O_CREAT, 0777, 0);
//    if (sem == SEM_FAILED)
//    {
//        std::cout << "ERROR CREATE SEM" << std::endl;
//    }
//
//    int val;
//    timespec tm;
//    sem_getvalue(sem, &val);
//    std::cout << val << std::endl;
//    sem_post(sem);
//    //sem_post(sem);
//    sem_getvalue(sem, &val);
//    std::cout << val << std::endl;
//    std::cout << "Try wait" << std::endl;
//
//    clock_gettime(CLOCK_REALTIME, &tm);
//    tm.tv_sec += 5;
//    while ((sem_timedwait(sem, &tm)) == -1 && errno == EINTR);
//    if (errno == ETIMEDOUT) std::cout << "TimeOut" << std::endl;
//
//    clock_gettime(CLOCK_REALTIME, &tm);
//    tm.tv_sec += 5;
//    while (( sem_timedwait(sem, &tm)) == -1 && errno == EINTR);
//    if (errno == ETIMEDOUT) std::cout << "TimeOut" << std::endl;
//
//
//    sem_getvalue(sem, &val);
//    std::cout << val << std::endl;
//    std::cout << "Wait done" << std::endl;
//
//    sem_close(sem);
//    sem_unlink("test_sem");
//
//    return 0;
//}

//int main(int ar, char** argv)
//{
//    std::cout << argv[1];
//
//    if (std::strcmp("create", argv[1]) == 0)
//    {
//        f_create();
//    }
//    else if (std::strcmp("read", argv[1]) == 0)
//    {
//        f_read();
//    }
//    else if (std::strcmp("crash", argv[1]) == 0)
//    {
//        f_crash();
//    }
//
//
//
//    return 0;
//}