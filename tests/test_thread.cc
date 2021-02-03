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
   main.uc_stack.ss_sp = stack2;//ָ��ջ�ռ�
   main.uc_stack.ss_size = sizeof(stack2);//ָ��ջ�ռ��С
   main.uc_stack.ss_flags = 0;
   ///

    
    getcontext(&child); //��ȡ��ǰ������
    child.uc_stack.ss_sp = stack;//ָ��ջ�ռ�
    child.uc_stack.ss_size = sizeof(stack);//ָ��ջ�ռ��С
    child.uc_stack.ss_flags = 0;
    child.uc_link = &main;//���ú��������
    //child.uc_link = nullptr;
    makecontext(&child, (void (*)(void))func1, 0);//�޸�������ָ��func1��
    swapcontext(&main, &child);//�л���child�����ģ����浱ǰ�����ĵ�main
    puts("main");//��������˺�������ģ�func1����ָ�����᷵�ش˴�
}



int main()
{
    context_test();
    puts("hahah");
    return 0;

}
