#include <iostream>
#include "Config_Reader.h"
#include "logger.h"
#include "Adapters.h"

class A
{
public:

	std::jthread j;
	void f()
	{
		auto st = j.get_stop_token();
		while (1)
		{
			if (st.stop_requested()) break;
			
			std::cout << st.stop_possible() <<"!!! HARD WORD !!! BAM BAM BAM KOPAT RUDU " << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		std::cout << " I AM BAD BONDSLAVE  0.0 HARAKIRI HCHK" << std::endl;
		return;
	}

	void start()
	{
		j = std::jthread(&A::f, this);
	}

	~A()
	{
		j.request_stop();
	}
};


int main(int argc, char** argv)
{
	std::shared_ptr<A> a = std::make_shared<A>();
	a->start();
	std::this_thread::sleep_for(std::chrono::seconds(8));
	//std::cout << a->j.request_stop() << std::endl;
}
