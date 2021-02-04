#include "sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run_in_fiber() {
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber begin";
    sylar::Fiber::YieldToHold();
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber end";
    sylar::Fiber::YieldToHold();
}

int main(int argc, char** argv) {

    // Æô¶¯main Ð­³Ì
    sylar::Fiber::getThis();
    SYLAR_LOG_INFO(g_logger) << "main fiber";
    sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber,1024*64));

    fiber->swapIn();

    SYLAR_LOG_INFO(g_logger) << "main after swapin";

    fiber->swapIn();

    SYLAR_LOG_INFO(g_logger) << "main after end";

    return 0;

}
