#include <iostream>
#include <mutex>
#include <thread>

#include "SingletonWithLockImpl.h"
#include "SingletonWithCallOnceImpl.h"
#include "testForkWhenMultiThreads.h"

void testSingletonWithLockImpl()
{
    auto threadFunc = []()  {
        std::cout<<"\n Thread-2: Calling getSingleton1Instance() 10 times: " << std::endl;
        for(int i=0; i< 10; ++i)
        {
            SingletonWithLockImpl::GetInstance();
        }
    };

    std::thread t(threadFunc);
    std::cout<<"\n Thread-1: Calling getSingleton1Instance() 10 times: " << std::endl;
    for(int i=100; i< 110; ++i)
    {
        SingletonWithLockImpl::GetInstance();
    }

    t.join();

}

void testSingletonWithCallOnceImpl()
{
    auto threadFunc = []()  {
        std::cout<<"\n Thread-2: Calling getSingleton2Instance() 10 times: " << std::endl;
        for(int i=0; i< 10; ++i)
        {
            SingletonWithCallOnceImpl::GetInstance();
        }
    };

    std::thread t(threadFunc);
    std::cout<<"\n Thread-1: Calling getSingleton2Instance() 10 times: " << std::endl;
    for(int i=100; i< 110; ++i)
    {
        SingletonWithCallOnceImpl::GetInstance();
    }

    t.join();

}


int main(int, char**){
    testForkWhenMultiThreads();
    // testSingletonWithLockImpl();
    // testSingletonWithCallOnceImpl();
    std::cout << "Hello, from classic_examples_impl!\n";
}
