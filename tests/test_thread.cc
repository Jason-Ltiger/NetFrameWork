#include "sylar/sylar.h"
#include <unistd.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int count = 0;
//sylar::RWMutex s_mutex;
sylar::Mutex s_mutex;

void fun1() {
    SYLAR_LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName()
                             << " this.name: " << sylar::Thread::GetThis()->getName()
                             << " id: " << sylar::GetThreadId()
                             << " this.id: " << sylar::Thread::GetThis()->getId();

    for(int i = 0; i < 100000; ++i) {
        //sylar::RWMutex::WriteLock lock(s_mutex);
        sylar::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {
    while(true) {
        SYLAR_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while(true) {
        SYLAR_LOG_INFO(g_logger) << "========================================";
    }
}
#if 0
int main(int argc, char** argv) {
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/sylar/test/sylar/bin/conf/log2.yml");
    sylar::Config::LoadFromYaml(root);

    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 2; ++i) {
        sylar::Thread::ptr thr(new sylar::Thread(&fun2, "name_" + std::to_string(i * 2)));
        sylar::Thread::ptr thr2(new sylar::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }

    for(size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "thread test end";
    SYLAR_LOG_INFO(g_logger) << "count=" << count;
    return 0;
}
#endif

#include <ucontext.h>
#include <stdio.h>

void func1(void* arg)
{
    puts("1");
    puts("11");
    puts("111");
    puts("1111");
}

void context_test()
{
    char stack[1024 * 128];
    
    ucontext_t child, main;
    ///
   char stack2[1024 * 128];
   main.uc_stack.ss_sp = stack2;//指定栈空间
   main.uc_stack.ss_size = sizeof(stack2);//指定栈空间大小
   main.uc_stack.ss_flags = 0;
   ///

    
    getcontext(&child); //获取当前上下文
    child.uc_stack.ss_sp = stack;//指定栈空间
    child.uc_stack.ss_size = sizeof(stack);//指定栈空间大小
    child.uc_stack.ss_flags = 0;
    child.uc_link = &main;//设置后继上下文
    //child.uc_link = nullptr;
    makecontext(&child, (void (*)(void))func1, 0);//修改上下文指向func1函
    swapcontext(&main, &child);//切换到child上下文，保存当前上下文到main
    puts("main");//如果设置了后继上下文，func1函数指向完后会返回此处
}



int main()
{
    context_test();
    puts("hahah");
    return 0;

}
