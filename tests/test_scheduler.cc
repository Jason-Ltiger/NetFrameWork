#include "sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

//void test_fiber() {
//    //static int s_count = 5;
//    static int s_count = 1;
//    SYLAR_LOG_INFO(g_logger) << "test in fiber s_count=" << s_count;
//
//    sleep(1);
//    if(--s_count >= 0) {
//        sylar::Scheduler::GetThis()->schedule(&test_fiber, sylar::GetThreadId());
//    }
//}
//
//void test_fiber1() {
//    //static int s_count = 5;
//    static int s_count = 1;
//    SYLAR_LOG_INFO(g_logger) << "test in fiber s_count=" << s_count;
//
//    sleep(1);
//    if (--s_count >= 0) {
//        sylar::Scheduler::GetThis()->schedule(&test_fiber1, sylar::GetThreadId());
//    }
//}

//test 0  ：一个线程，无执行的任务fiber
//创建当前线程就是root_fiber的线程。
//start创建0个额外线程
//如若队列里面没有额外的协程，那么直接停止，
//如若队列里面有额外的协程任务，stop的时候会调用root_fiber去执行回收功能。
//int main(int argc, char** argv) {   
//    sylar::Scheduler sc;
//    sc.start();
//    sc.schedule(&test_fiber1);
//    sc.stop();
//    return 0;
//}

//
////test 1  感觉这方方式在异步创建执行协程的方式更清晰。
//int main(int argc, char** argv) {
//    SYLAR_LOG_INFO(g_logger) << "main";
//    线程的主协程，就用来当作主线。就没有m_root_fiber
//    sylar::Scheduler sc(1, false, "test");
//    创建执行线程
//    sc.start();
//    //sleep(1000000);
//    sleep(2);
//    SYLAR_LOG_INFO(g_logger) << "schedule";
//    //协程压入队列。
//    sc.schedule(&test_fiber);
//    sleep(1000000);
//    sc.stop();
//    SYLAR_LOG_INFO(g_logger) << "over";
//    return 0;
//}


void test_fiber() {

    SYLAR_LOG_INFO(g_logger) << "test in fiber ";
    sleep(1);
}



//test 2  创建多个线程去执行。
int main(int argc, char** argv) {
    sylar::Scheduler sc(3);
    sc.start();
    sc.schedule(&test_fiber);
    sc.stop();
    return 0;
}