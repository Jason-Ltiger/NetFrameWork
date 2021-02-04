#include "sylar/scheduler.h"
#include "sylar/log.h"

namespace sylar;
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
virtual ~Scheduler();


static Scheduler* GetThis();
static Fiber* GetMainFiber();

void start();
void stop();