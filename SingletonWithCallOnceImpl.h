#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <cassert>

class SingletonWithCallOnceImpl
{
public:
    static SingletonWithCallOnceImpl* GetInstance()
    {
        std::call_once(onceFlag_, 
        [&]()->void
        {
            //if(instance_ == nullptr)
                instance_ = new SingletonWithCallOnceImpl();
        }
        );

        return instance_;
    }

private:
    SingletonWithCallOnceImpl()
    {
        times_++;
        assert(times_==1);
    }

private:
    static SingletonWithCallOnceImpl* instance_;
    static std::once_flag onceFlag_;
    static int times_;
};

//init
SingletonWithCallOnceImpl* SingletonWithCallOnceImpl::instance_ = nullptr;
int SingletonWithCallOnceImpl::times_ = 0;
std::once_flag SingletonWithCallOnceImpl::onceFlag_;


