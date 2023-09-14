#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <cassert>

class SingletonWithLockImpl
{
public:
    SingletonWithLockImpl()
    {
        times_++;
        assert(times_==1);
    }

    static SingletonWithLockImpl* GetInstance()
    {
        //double check
        if(instance_ == nullptr)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            if(instance_  == nullptr)
            {
                instance_ = new SingletonWithLockImpl();
            }
        }

        return instance_;
    }

private:
    static SingletonWithLockImpl* instance_;
    static std::mutex mutex_;
    static int times_;
};

//init
SingletonWithLockImpl* SingletonWithLockImpl::instance_ = nullptr;
std::mutex SingletonWithLockImpl::mutex_;
int SingletonWithLockImpl::times_ = 0;


