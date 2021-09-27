// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorld_main.cpp
 *
 */

#include "HelloWorldPublisher.h"
#include "HelloWorldSubscriber.h"


int main(int argc, char** argv)
{
    std::cout << "Starting "<< std::endl;
    int type = 1;
    int count = 5;
    long sleep = 100;

    HelloWorldPublisher mypub;
    if (mypub.init())
    {
        mypub.run(count, sleep);
    }
    
    /*HelloWorldSubscriber mysub;
    if (mysub.init())
    {
        mysub.run();
    }*/

    for (;;)
    {
        std::this_thread::sleep_for(2000ms);
    }
    
    return 0;
}
