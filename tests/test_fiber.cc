//#include "sylar/sylar.h"
//
//sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
//
//void run_in_fiber() {
//    SYLAR_LOG_INFO(g_logger) << "run_in_fiber begin";
//    sylar::Fiber::YieldToHold();
//    SYLAR_LOG_INFO(g_logger) << "run_in_fiber end";
//    sylar::Fiber::YieldToHold();
//}
//
//int main(int argc, char** argv) {
//
//    // Æô¶¯main Ð­³Ì
//    sylar::Fiber::getThis();
//    SYLAR_LOG_INFO(g_logger) << "main fiber";
//    sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber,1024*64));
//
//    fiber->swapIn();
//
//    SYLAR_LOG_INFO(g_logger) << "main after swapin";
//
//    fiber->swapIn();
//
//    SYLAR_LOG_INFO(g_logger) << "main after end";
//
//    return 0;
//
//}

#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <string.h>

ucontext_t uc, ucm;
void foo()
{
    int a = 5;
    printf("%d\n", a);
    char ptr[128]{ 0 };
    ptr[0] = 'a';
    ptr[1] = 'b';

    void* p = (void*)ptr;
    printf("%s\n", __FUNCTION__);
    printf("%p\n", p);
}

int main()
{
    // allocate stack
    size_t co_stack_size = 256;
    //char* co_stack = (char*)malloc(co_stack_size);
    char co_stack[256];
    memset(co_stack, 0, co_stack_size);

    //get current context
    getcontext(&uc);

    // make ucontext to run foo
    uc.uc_stack.ss_sp = co_stack;
    uc.uc_stack.ss_size = co_stack_size;
    uc.uc_link = &ucm;
    makecontext(&uc, foo, 0);

    // switching back-and-forth for 100 times

    swapcontext(&ucm, &uc);


    free(co_stack);
    return 0;
}
