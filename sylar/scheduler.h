#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__ 
#include <memory>
#include "fiber.h"
#include "thread.h"

namespace sylar {

    //将协程指定到指定的线程上去执行。
    class Scheduler {

    public:
        typedef std::shared_ptr<Scheduler> ptr;
        typedef sylar::Mutex MutexType;

    public:
        //线程的大小，默认开启使用一个线程
        //在哪里调用这个调度器，那么就把这个线程加入进来。
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

         //批量放置FiberOrCb放入到容器中
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
        // 一个一个的把FiberOrCb放入到容器中
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
            // 使用智能指针的指针。这样就能把智能swap之后减1.智能指针的智能指针就指向为空。
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
