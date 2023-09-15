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

#define THREADPOOL_MAX_NUM 16
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

public:
    threadpool(size_t size = 4)
        :run_(true)
        ,idleThreadNum_(0)
        ,initSize_(size)
    {
        startThread(size);
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

    int idleThreadNum() const {return idleThreadNum_;}
    int allThreadNum() const {return threads_.size();}

    //启动size线程
    void startThread(size_t size)
    {
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
                        idleThreadNum_--;//atomic?
                        curTask=std::move(tasksQueue_.front());
                        tasksQueue_.pop();
                    }

                    //执行task
                    curTask();

                    idleThreadNum_++;//atomic?

                }
            };

            threads_.emplace_back(threadFunc);
            idleThreadNum_++;//atomic?
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
        taskCond_.notify_one();
    }
};