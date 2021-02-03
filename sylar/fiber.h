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
        //����Э�̺���
        void reset(std::function<void()> cb);
        // �л���ǰЭ��ִ��
        void swapIn();
        // �л�����ִ̨��
        void swapOut();
        //
        void* debugstack() { return m_stack; }
        uint64_t getId() const { return m_id; };

    public:
        //���õ�ǰЭ��
        static void setThis(Fiber * f);
        //���ص�ǰЭ��
        static Fiber::ptr getThis();
        //�ó�Э��ִ�е���̨����������Ϊready״̬
        static void YeildToReady();
        //�ó�Э��ִ�е���̨����������Ϊhold״̬
        static void YieldToHold();
        static uint64_t totalFibers();

        static void MainFunc();
        static uint64_t GetFiberId();
    private:
        //Э�̳�Ա����
        uint64_t              m_id = 0;
        uint32_t              m_stack_size = 0;
        State                 m_state = INIT;

        ucontext_t            m_ctx;
        void*                 m_stack = nullptr;
        std::function<void()> m_cb;
    };
}

#endif //__SYLAR_FIBER_H
