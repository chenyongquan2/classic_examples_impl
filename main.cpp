#include <iostream>
#include <mutex>
#include <string>
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

std::string addStrings(const std::string& num1, const std::string& num2)
{
    std::string result;
    int carry=0;
    int i= num1.size()-1;
    int j=num2.size()-1;
    while(i>=0||j>=0||carry>0)
    {
        int d1= i>=0?num1[i--]-'0':0;
        int d2=j>=0?num2[j--]-'0':0;
        int sum=d1+d2+carry;
        carry=sum/10;
        int curDigit = sum%10;
        result.push_back(curDigit+'0');
    }

    std::reverse(result.begin(),result.end());
    return result;
}

void testAddString()
{
    std::string num1 = "123456789";  
    std::string num2 = "987654321";  
    std::string sum = addStrings(num1, num2);  
    std::cout << "Sum: " << sum << std::endl;  
}

#include <QApplication>  
#include <QLabel>  

#include "testThreadPool.h"

int main(int argc, char** argv){
    testAddString();
    // QApplication app(argc, argv);  

    // QLabel label("Hello, World!");  
    // label.show();

    // return app.exec();  
    //testThreadPool();
    //testForkWhenMultiThreads();
    // testSingletonWithLockImpl();
    // testSingletonWithCallOnceImpl();
    std::cout << "Hello, from classic_examples_impl!\n";
}
