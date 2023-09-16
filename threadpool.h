#pragma once

#include <cstddef>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <functional>
#include <cassert>
#include <stdexcept>
#include <iostream>

#define THREADPOOL_MAX_NUM 16
#define THREADPOOL_AUTO_GROW 

class threadpool
{
private:
    size_t initSize_;
    using Task = std::function<void()>;
    std::vector<std::thread> threads_;
    std::queue<Task> tasksQueue_;
    std::mutex mutex_; //任务队列同步锁
    std::condition_variable taskCond_;//条件阻塞

    std::atomic<bool> run_;
    //std::atomic<bool> run_ = true;//某些编译器会编译不通过
    std::atomic<int> idleThreadNum_ ;//空闲线程数量

#ifdef THREADPOOL_AUTO_GROW
    std::mutex autoGrowMutex_;
#endif // THREADPOOL_AUTO_GROW

public:
    threadpool(size_t size = 4)
        :run_(true)
        ,idleThreadNum_(0)
        ,initSize_(size)
    {
        addAndStartThread(size);
    }
    ~threadpool()
    {
        run_ = false;
        taskCond_.notify_all();//唤醒所有线程

        for(std::thread& t: threads_)
        {
            if(t.joinable())
                t.join();
        }
    }

    int idleThreadNum() const {return idleThreadNum_.load();}
    int allThreadNum() const {return threads_.size();}

#ifndef THREADPOOL_AUTO_GROW
//当不允许自动增长，此方法也没必要暴露出去。
private:
#endif // !THREADPOOL_AUTO_GROW
    //启动size线程
    void addAndStartThread(size_t size)
    {
#ifdef THREADPOOL_AUTO_GROW
        //当可以增加线程时，需要在增加线程时进行加锁，防止多个线程同时来扩展线程池
        std::unique_lock<std::mutex> lockAutoGrow(autoGrowMutex_);
#endif // THREADPOOL_AUTO_GROW

        size_t newThreadSize = std::min(size, THREADPOOL_MAX_NUM - threads_.size());
        for(size_t i=0;i<newThreadSize;i++)
        {
            auto threadFunc = [this]() 
            { 
                while(true)
                {
                    Task curTask;
                    //减少锁的粒度
                    {
                        std::unique_lock<std::mutex> lockGuard(mutex_);
                        //第二个参数是一个返回bool类型的lambda，返回true表示可以唤醒返回。
                        taskCond_.wait(lockGuard, [this] {
                            return !run_ || !tasksQueue_.empty();
                        });

                        if(!run_ && tasksQueue_.empty())
                        {
                            //当mrun_但是tasksQueue_不为空，应该要把剩余的任务都给消费完，才退出线程
                            return;
                        }
                        //这里上面已经加锁保证了。
                        idleThreadNum_--;//atomic?
                        curTask=std::move(tasksQueue_.front());
                        tasksQueue_.pop();
                    }

                    //执行task
                    curTask();
                    idleThreadNum_.fetch_add(1);
                    //idleThreadNum_++;//atomic?

                }
            };

            threads_.emplace_back(threadFunc);
            idleThreadNum_.fetch_add(1);
            //idleThreadNum_++;//atomic?
        }
    }

    //运用可变参数模板+实现类型擦除的效果
    template<typename Func, typename... Args>
    void addTask(Func&& f, Args&&... args)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasksQueue_.emplace(
            [=]()
            {
                //或者
                //f(std::forward<Args>(args)...);

                std::invoke(f,args...);
                //std::invoke是c++17的特性，因此在cmakelists.txt中
                // #支持c++20标准
                // set(CMAKE_CXX_STANDARD 20)
            }
        );

        
#ifdef THREADPOOL_AUTO_GROW
        //判断是否要增加多1个线程
        std::cout << "[addTask] idleThreadNum_="<<idleThreadNum_<<",allThreadNum="<<threads_.size()<<std::endl;
        if(idleThreadNum_.load() < 1 && threads_.size() < THREADPOOL_MAX_NUM)//上面已经加锁了，已经保证了threads.size()处于锁的范围内了。
        {
            addAndStartThread(1);
            std::cout << "[addTask] has expand a new thread, now threadSize:" << threads_.size()<<std::endl;
        }
            
#endif // THREADPOOL_AUTO_GROW

        //唤醒一个线程执行
        taskCond_.notify_one();
    }
};