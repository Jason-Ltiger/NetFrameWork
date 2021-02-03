#include "fiber.h"
#include "config.h"
#include "log.h"
#include "macro.h"
#include <atomic>

namespace sylar {

    static Logger::ptr g_logger = SYLAR_LOG_NAME("system");
    static std::atomic<uint64_t> s_fiber_id{ 0 };
    static std::atomic<uint64_t> s_fiber_count{ 0 };
    //当前线程执行的协程
    static thread_local Fiber* t_fiber = nullptr;
    Fiber::ptr t_threadFiber = nullptr;
    static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
        Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

    //内存分配器
    class MallocStackAllocator {

    public:
        static void* Alloc(size_t size) {
            return malloc(size);
        }
        static void Delloc(void *vp,size_t size) {
            free(vp);
            return;
        }
    };
    using StackAllocator = MallocStackAllocator;

    //主协程
    Fiber::Fiber() {
        m_state = EXEC;
        setThis(this);
        if (getcontext(&m_ctx)) {
            SYLAR_ASSERT2(false, "getcontext");
        }
        ++s_fiber_id;
        SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber id" << m_id;
    };
    //真正创建协程
    Fiber::Fiber(std::function<void()> cb, size_t stacksize)
    :m_id(++s_fiber_id)
    ,m_cb(cb){
        m_stack_size = stacksize != 0 ? stacksize : g_fiber_stack_size->getValue();
        //分配内存大小
        m_stack = StackAllocator::Alloc(m_stack_size);
        if (getcontext(&m_ctx)) {
            SYLAR_ASSERT2(false, "getcontext");
        }
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stack_size;
        SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber id" << m_id;
        makecontext(&m_ctx, &Fiber::MainFunc,0);

    }
    //协程结束后
    Fiber::~Fiber(){
        --s_fiber_count;
        //分为main协程与普通协程的析构
        if (m_stack) {
            SYLAR_ASSERT(m_state == TERM || m_state == INIT);
            StackAllocator::Delloc(m_stack, m_stack_size);
        }
        else {
            SYLAR_ASSERT(!m_cb);
            SYLAR_ASSERT(m_state == EXEC);
            Fiber* cur = t_fiber;
            if (cur == this) {
                setThis(nullptr);
            }
        }
        SYLAR_LOG_DEBUG(g_logger) << "Fiber::~Fiber id" << m_id;
    } ;

    //重置协程函数，基于这块执行栈，执行新的协程
    void Fiber::reset(std::function<void()> cb) {
        //判断协程条件
        SYLAR_ASSERT(m_stack);
        SYLAR_ASSERT(m_state == INIT || m_state == TERM);

        //设置回调
        m_cb = cb;
        //重置ucontext对象
        if (getcontext(&m_ctx)) {
            SYLAR_ASSERT2(false,"getcontext");
        }
        //重新利用前协程的栈空间
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_size = m_stack_size;
        m_ctx.uc_stack.ss_sp = m_stack;
        //更新m_ctx 协程函数。
        makecontext(&m_ctx,&Fiber::MainFunc,0);
        m_state = INIT;

    };
    // 当前执行协程放到后台,执行当前协程。
    void Fiber::swapIn() {
        setThis(this);
        SYLAR_ASSERT(m_state != EXEC);
        m_state = EXEC;
        // 保存mian协程的上下文，执行新的协程的上下文  t_threadFiber 就是main协程
        if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
            SYLAR_ASSERT2(false,"swapcontext");
        }
    };
    // 把当前切换到后台执行，切换main协程
    void Fiber::swapOut() {
        setThis(t_threadFiber.get());
        if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
            SYLAR_ASSERT2(false,"swapcontext");
        }
    };

    //设置当前协程
    void Fiber::setThis(Fiber* f) {
        t_fiber = f;
    
    };
    //返回当前协程
     Fiber::ptr Fiber::getThis(){
         if (t_fiber) {
             return t_fiber->shared_from_this();
         }
         Fiber::ptr main_fiber(new Fiber);
         SYLAR_ASSERT(t_fiber == main_fiber.get());
         //智能指针的智能指针
         t_threadFiber = main_fiber;
         return t_fiber->shared_from_this();
     };
    //让出协程执行到后台，并且设置为ready状态
     void Fiber::YeildToReady() {
         Fiber::ptr cur = getThis();
         cur->m_state = READY;
         cur->swapOut();
     };
    //让出协程执行到后台，并且设置为hold状态
     void Fiber::YieldToHold() {
         Fiber::ptr cur = getThis();
         cur->m_state = HOLD;
         cur->swapOut();
     };
     uint64_t Fiber::totalFibers() {
         return s_fiber_count;
     };

     void Fiber::MainFunc() {
         Fiber::ptr cur = getThis();
         SYLAR_ASSERT(cur);

         try {
             cur->m_cb();
             cur->m_cb = nullptr;
             cur->m_state = TERM;
         }
         catch (std::exception& ex) {
             cur->m_state = EXCEPT;
             SYLAR_LOG_ERROR(g_logger) << "Fiber Except" << ex.what();
         
         }catch(...) {
             cur->m_state = EXCEPT;
             SYLAR_LOG_ERROR(g_logger) << "Fiber Except" ;
         
         }


     }
     uint64_t Fiber::GetFiberId(){
         if (t_fiber) {
             return t_fiber->getId();
         }
         return 0;
     };


}

