#ifndef  __SYLAR_FIBER_H
#define  __SYLAR_FIBER_H


#include <memory>
#include <functional>
#include <ucontext.h>
#include "thread.h"

namespace sylar {

    class Fiber : public std::enable_shared_from_this<Fiber> {

    public:
        typedef std::shared_ptr<Fiber> ptr;
        enum State {
            INIT,
            HOLD,
            EXEC,
            TERM,
            READY,
            EXCEPT
        };

    private:
        Fiber();
    public:
        Fiber(std::function<void()> cb, size_t stacksize = 0);
        ~Fiber();

    public:
        //重置协程函数
        void reset(std::function<void()> cb);
        // 切换当前协程执行
        void swapIn();
        // 切换到后台执行
        void swapOut();
        //
        void* debugstack() { return m_stack; }
        uint64_t getId() const { return m_id; };

    public:
        //设置当前协程
        static void setThis(Fiber * f);
        //返回当前协程
        static Fiber::ptr getThis();
        //让出协程执行到后台，并且设置为ready状态
        static void YeildToReady();
        //让出协程执行到后台，并且设置为hold状态
        static void YieldToHold();
        static uint64_t totalFibers();

        static void MainFunc();
        static uint64_t GetFiberId();
    private:
        //协程成员变量
        uint64_t              m_id = 0;
        uint32_t              m_stack_size = 0;
        State                 m_state = INIT;

        ucontext_t            m_ctx;
        void*                 m_stack = nullptr;
        std::function<void()> m_cb;
    };
}

#endif //__SYLAR_FIBER_H
