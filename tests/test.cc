#include <iostream>
#include "sylar/log.h"
#include "sylar/util.h"
#include <ucontext.h>
#include <unistd.h>

#if 0
int main(int argc, char** argv) {
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::FileLogAppender::ptr file_appender(new sylar::FileLogAppender("./log.txt"));
    sylar::LogFormatter::ptr fmt(new sylar::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(sylar::LogLevel::ERROR);

    logger->addAppender(file_appender);

    //sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0, sylar::GetThreadId(), sylar::GetFiberId(), time(0)));
    //event->getSS() << "hello sylar log";
    //logger->log(sylar::LogLevel::DEBUG, event);
    std::cout << "hello sylar log" << std::endl;

    SYLAR_LOG_INFO(logger) << "test macro";
    SYLAR_LOG_ERROR(logger) << "test macro error";

    SYLAR_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    SYLAR_LOG_INFO(l) << "xxx";
    return 0;
}
#endif
void func1(void* arg)
{
    puts("1");
    puts("11");
    puts("111");
    puts("1111");
}
void context_test(){
    char stack[1024 * 128];
    ucontext_t child, main;
    getcontext(&child); //��ȡ��ǰ������
    child.uc_stack.ss_sp = stack;//ָ��ջ�ռ�
    child.uc_stack.ss_size = sizeof(stack);//ָ��ջ�ռ��С
    child.uc_stack.ss_flags = 0;
    child.uc_link = &main;//���ú��������
    printf("%p", func1);
    makecontext(&child, (void (*)(void))func1, 0);//�޸�������ָ��func1����
    swapcontext(&main, &child);//�л���child�����ģ����浱ǰ�����ĵ�main
    puts("main");//��������˺�������ģ�func1����ָ�����᷵�ش˴�
}

int main() {

    context_test();
    /*ucontext_t context;
    getcontext(&context);
    sleep(1);*/

    return 0;
}
