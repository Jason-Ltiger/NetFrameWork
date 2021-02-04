#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__ 
#include <memory>
#include "fiber.h"
#include "thread.h"

namespace sylar {

    //��Э��ָ����ָ�����߳���ȥִ�С�
    class Scheduler {

    public:
        typedef std::shared_ptr<Scheduler> ptr;
        typedef sylar::Mutex MutexType;

    public:
        //�̵߳Ĵ�С��Ĭ�Ͽ���ʹ��һ���߳�
        //����������������������ô�Ͱ�����̼߳��������
        Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
        virtual ~Scheduler();
    public:
        static Scheduler* GetThis();
        static Fiber* GetMainFiber();
    
    public:
        void start();
        void stop();

         template<class FiberOrCb>
         void schedule(FiberOrCb fc, int thread = -1) {
             bool need_tickle = false;
             {
                MutexType::Lock lock(m_mutex);
                need_tickle = sheduleNoLock(fc,thread);
             }
             if (need_tickle) {
                 tickie();
             }
         }

         //��������FiberOrCb���뵽������
         template<class InputIterator>
         void schedule(InputIterator begin, InputIterator end) {
             bool need_tickle = false;
             {
                 while (begin!=end) {
                     need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                     ++begin;
                 }
             }
             if (need_tickle) {
                 tickle();
             }
         }

    protected:
        virtual void tickie();
    private:
        // һ��һ���İ�FiberOrCb���뵽������
        template<class FiberOrCb>
        bool sheduleNoLock(FiberOrCb fc, int thread) {
            bool need_tickie = m_fibers.empty();
            FiberAndThread ft(fc, thread);
            if (ft.fiber || ft.cb) {
                m_fibers.push_back(ft);
            }
            return need_tickie;

        }

    private:
        struct FiberAndThread {

            Fiber::ptr fiber;
            std::function<void()> vb;
            int32_t thread;
            FiberAndThread(Fiber::ptr f, int32_t thr)\
                :fiber(f), thread(thr){}
            // ʹ������ָ���ָ�롣�������ܰ�����swap֮���1.����ָ�������ָ���ָ��Ϊ�ա�
            FiberAndThread(Fiber::ptr* f, int32_t thr)\
                : thread(thr){
                fiber.swap(*f);
            }
            FiberAndThread(std::function<void()> *f,int32_t thr)\
                : thread(thr) {
                vb.swap(*f);
            }
            FiberAndThread() :thread(-1) {};

            void reset() {
                thread = -1;
                vb = nullptr;
                fiber = nullptr;
            }

        };
    private:
        MutexType m_mutex;
        std::vector<sylar::Thread::ptr> m_threads;
        const std::string m_name;
        std::list <FiberAndThread> m_fibers;

    };

}


#endif//__SYLAR_SCHEDULER_H__
