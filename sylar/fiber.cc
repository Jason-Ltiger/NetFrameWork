#include "fiber.h"
#include "config.h"
#include "log.h"
#include "macro.h"
#include <atomic>

namespace sylar {

    static Logger::ptr g_logger = SYLAR_LOG_NAME("system");
    static std::atomic<uint64_t> s_fiber_id{ 0 };
    static std::atomic<uint64_t> s_fiber_count{ 0 };
    //��ǰ�߳�ִ�е�Э��
    static thread_local Fiber* t_fiber = nullptr;
    Fiber::ptr t_threadFiber = nullptr;
    static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
        Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

    //�ڴ������
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

    //��Э��
    Fiber::Fiber() {
        m_state = EXEC;
        setThis(this);
        if (getcontext(&m_ctx)) {
            SYLAR_ASSERT2(false, "getcontext");
        }
        ++s_fiber_id;
        SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber id" << m_id;
    };
    //��������Э��
    Fiber::Fiber(std::function<void()> cb, size_t stacksize)
    :m_id(++s_fiber_id)
    ,m_cb(cb){
        m_stack_size = stacksize != 0 ? stacksize : g_fiber_stack_size->getValue();
        //�����ڴ��С
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
    //Э�̽�����
    Fiber::~Fiber(){
        --s_fiber_count;
        //��ΪmainЭ������ͨЭ�̵�����
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

    //����Э�̺������������ִ��ջ��ִ���µ�Э��
    void Fiber::reset(std::function<void()> cb) {
        //�ж�Э������
        SYLAR_ASSERT(m_stack);
        SYLAR_ASSERT(m_state == INIT || m_state == TERM);

        //���ûص�
        m_cb = cb;
        //����ucontext����
        if (getcontext(&m_ctx)) {
            SYLAR_ASSERT2(false,"getcontext");
        }
        //��������ǰЭ�̵�ջ�ռ�
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_size = m_stack_size;
        m_ctx.uc_stack.ss_sp = m_stack;
        //����m_ctx Э�̺�����
        makecontext(&m_ctx,&Fiber::MainFunc,0);
        m_state = INIT;

    };
    // ��ǰִ��Э�̷ŵ���̨,ִ�е�ǰЭ�̡�
    void Fiber::swapIn() {
        setThis(this);
        SYLAR_ASSERT(m_state != EXEC);
        m_state = EXEC;
        // ����mianЭ�̵������ģ�ִ���µ�Э�̵�������  t_threadFiber ����mainЭ��
        if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
            SYLAR_ASSERT2(false,"swapcontext");
        }
    };
    // �ѵ�ǰ�л�����ִ̨�У��л�mainЭ��
    void Fiber::swapOut() {
        setThis(t_threadFiber.get());
        if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
            SYLAR_ASSERT2(false,"swapcontext");
        }
    };

    //���õ�ǰЭ��
    void Fiber::setThis(Fiber* f) {
        t_fiber = f;
    
    };
    //���ص�ǰЭ��
     Fiber::ptr Fiber::getThis(){
         if (t_fiber) {
             return t_fiber->shared_from_this();
         }
         Fiber::ptr main_fiber(new Fiber);
         SYLAR_ASSERT(t_fiber == main_fiber.get());
         //����ָ�������ָ��
         t_threadFiber = main_fiber;
         return t_fiber->shared_from_this();
     };
    //�ó�Э��ִ�е���̨����������Ϊready״̬
     void Fiber::YeildToReady() {
         Fiber::ptr cur = getThis();
         cur->m_state = READY;
         cur->swapOut();
     };
    //�ó�Э��ִ�е���̨����������Ϊhold״̬
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

