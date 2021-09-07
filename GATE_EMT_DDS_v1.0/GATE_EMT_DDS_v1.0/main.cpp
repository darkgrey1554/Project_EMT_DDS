#include <iostream>
#include "Config_Reader.h"
#include <memory>
#include <ctime>
#include <chrono>
#include "KKS_Reader.h"

char* c =new char(10);

std::unique_ptr<void*> f()
{
	
	std::unique_ptr<void*> v = std::make_unique<void*>((void*)c);
	return std::move(v);
}


int main()
{
	KKSReader* reader = new KKSReader();

	reader->ReadKKSlist("List_KKS_In.txt");
	

	for (int i = 0; i < 10; i++)
	{
		c[i] = i+0x30;
	}

	std::unique_ptr<void*> a = f();

	char* s = (char*)*a.get();

	std::cout << *s << std::endl;
	std::cout << *(s+1) << std::endl;


	ConfigReaderDDS* conf = new ConfigReaderDDS();
	conf->ReadConfigDDS();

	std::cout << "Hello" <<std::endl;

}
