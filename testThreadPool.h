#include "threadpool.h"
#include <iostream>
#include <stdexcept>
#include <thread>

void fun1(int ms)
{
	std::cout << "[fun1] tid="  << std::this_thread::get_id() << std::endl;
	if (ms>0) {
        std::cout << " [fun1] sleep tid="  << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
}

struct StructA {
	int operator()(int n) {
        std::cout << " [StructA::operator()] tid="  << std::this_thread::get_id() << std::endl;
		return 42;
	}
};

class ClassA {    //函数必须是 static 的才能使用线程池
public:
	int normalFunc(int n = 0) {
		std::cout << n << "  [ClassA::normalFunc] tid=" << std::this_thread::get_id() << std::endl;
		return n;
	}

	static std::string staticFunc(int n, std::string str, char c) {
		std::cout << n << " [ClassA::staticFunc] tid= "<< std::this_thread::get_id() << "str=" << str.c_str() <<" ,c=" << (int)c << std::endl;
		return str;
	}
};

void testThreadPool()
{
    threadpool executor(10);

    executor.addTask(fun1, 0);

    // StructA objStructA;
    // executor.addTask(objStructA, 0);

    ClassA objA;
    executor.addTask(&ClassA::normalFunc, &objA, 9999);
    executor.addTask(&ClassA::staticFunc, 9998,"mult args", 123);

    std::cout << "[main thread] tid = " << std::this_thread::get_id() << ",idelThreadsize = "<<executor.idleThreadNum() << " is free!"<< std::endl;
	std::this_thread::sleep_for(std::chrono::microseconds(3000));
    std::cout << "[main thread] tid = " << std::this_thread::get_id() << ",idelThreadsize = "<<executor.idleThreadNum() << " is free!"<< std::endl;

    for (int i = 0; i < 50; i++) {
        executor.addTask(fun1,i*100);
    }

    std::cout << "[main thread] tid = " << std::this_thread::get_id() << ",idelThreadsize = "<<executor.idleThreadNum() << " is free!"<< std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "[main thread] tid = " << std::this_thread::get_id() << ",idelThreadsize = "<<executor.idleThreadNum() << " is free!"<< std::endl;

    //在这里threadpool对象会被析构，会join所有子线程结束。
}