#include "sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run_in_fiber() {
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber begin";
    sylar::Fiber::YieldToHold();
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber end";
    sylar::Fiber::YieldToHold();
}

void test_fiber() {
    SYLAR_LOG_INFO(g_logger) << "main begin -1";
    {
        sylar::Fiber::getThis();
        SYLAR_LOG_INFO(g_logger) << "main begin";
        sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber));
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    SYLAR_LOG_INFO(g_logger) << "main after end2";
}

//int main(int argc, char** argv) {
//    sylar::Thread::SetName("main");
//
//    std::vector<sylar::Thread::ptr> thrs;
//    for (int i = 0; i < 2; ++i) {
//        thrs.push_back(sylar::Thread::ptr(
//            new sylar::Thread(&test_fiber, "name_" + std::to_string(i))));
//    }
//    for (auto i : thrs) {
//        i->join();
//    }
//    return 0;
//}

#include <iostream>
void add(int a, int b) {
    std::cout << __FUNCTION__ << std::endl;
};
void sub(int a, int b) {
    std::cout << __FUNCTION__ << std::endl;
};

std::function<void()> cb1;
std::function<void()> cb2;
int main(int argc, char** argv) {

    cb1();
    cb1 = std::bind(add,5,6);
    cb2 = std::bind(sub, 5, 6);
    cb1();
    cb2();
    cb1.swap(cb2);
    std::cout << "swap after --- " << std::endl;
    cb1();
    cb2();
    std::cout << (void*)&cb1 << std::endl;
    cb1 = nullptr;
    cb2 = nullptr;
    std::cout << (void*)&cb1 << std::endl;
    std::cout << "swap after --- " << std::endl;
    cb1();
    cb2();

}

