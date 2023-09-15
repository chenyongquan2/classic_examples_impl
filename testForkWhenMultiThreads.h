
#include <unistd.h>
#include <iostream>
#include "SingletonWithLockImpl.h"
#include "SingletonWithCallOnceImpl.h"
#include <mutex>
#include <thread>

//https://zhuanlan.zhihu.com/p/130873706
// 刚刚尝试自个去写代码去分析梳理并验证了一下上述多进程多线程模型的问题，欢迎一起探讨：多线程下 若主线程去进行fork，仅会将发起调用的线程复制到子进程中(子进程中该线程的ID与父进程中发起fork()调用的线程ID是一样的
// )，
// 其他线程均在子进程中立即停止并消失，并且不会为这些线程调用清理函数以及针对线程局部存储变量的析构函数；
// 在上面的模型中，在fork之前，主进程的子线程去加锁了锁，但是fork后，子进程的子线程却消失了(也就是说子进程没有子线程的逻辑去执行后续的unlock操作)，
// 如果这个子进程在后续执行仍去加锁lock，那么就会阻塞在lock上。
// 那么等到主进程的子线程去unlock解锁了，子进程的锁状态会不会得到解锁呢？能不能抢救呢？
// 很遗憾，不会。
// 因为fork的写时复制的机制，当在子进程执行lock，属于写操作，是会把锁给copy一份的(注意此时锁是处于加锁状态)，
// 如果后续父进程的子线程对这个锁去解锁了，但由于已经copy一份了，也不会影响到子进程的锁的状态，
// 这样子进程的锁就一直处于加锁，再也没有任何的好心人去给这个锁来解锁了，只能等到超时进程被杀，就crash了。

void testForkWhenMultiThreads()
{
    std::mutex m;

    auto threadFunc = [&]() {
        m.lock();
        std::cout << "[parent process][child Thread] tid:" << std::this_thread::get_id() << " has lock!\n";
        //模拟在主进程的子线程去加锁
        sleep(10);//睡眠一段时间，确保fork的时候，锁m是处于加锁的状态
        m.unlock();
        std::cout << "[parent process][child Thread] tid:" << std::this_thread::get_id() << " has unlock!\n";
    };

    std::thread t(threadFunc);
    sleep(3);//sleep一段时间，确保此时主进程的子线程已经对锁m进行了加锁状态，再去fork

    std::cout << "[parent process][parent Thread] tid:" << std::this_thread::get_id() << " begin to fork!\n";

    //在主进程的主线程去开启fork
    pid_t pid = fork();
    assert(pid>=0);
    if(pid == 0)
    {
        //子进程,此时子进程是没有多线程的，只有父进程
        std::cout << "[child process] tid:" << std::this_thread::get_id() << " begin lock!\n";
        //会去走写时复制机制，锁m会被拷贝一份,得到锁m1,锁m1此时是加锁状态的
        m.lock();//没办法解锁，一直阻塞在这里。
        std::cout << "[child process] tid:" << std::this_thread::get_id() << " end lock!\n";

        std::cout << "[child process] tid:" << std::this_thread::get_id() << " begin unlock!\n";
        m.unlock();
        std::cout << "[child process] tid:" << std::this_thread::get_id() << " end unlock!\n";

        std::cout << "[child process]tid:" << std::this_thread::get_id() << " done!\n";
    }
    else if(pid > 0)
    {
        //父进程
        sleep(100000);
        std::cout << "[parent process] finish sleep!\n";
        t.join();
        std::cout << "[parent process] done!\n";
    }
}