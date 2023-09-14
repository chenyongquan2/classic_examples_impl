#include <iostream>
#include "SingletonWithLockImpl.h"
#include <thread>


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

int main(int, char**){
    testSingletonWithLockImpl();
    std::cout << "Hello, from classic_examples_impl!\n";
}
