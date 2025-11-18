#include "AsyncEvenLoopInvoker.h"
#include "EventLoopWorker.h"

#include <QThread>
#include <QObject>
#include <atomic>

BEGIN_COMMON_TOOLS_SPACE

// 宏定义：安全地获取实现对象指针
#define OBTAIN_OBJ(worker, objImpl, TypeName); auto worker = (TypeName*)objImpl;

bool AsyncEventLoopInvoker::isBusy()
{
    OBTAIN_OBJ(worker, m_objImpl, EventLoopWorker);  // 获取EventLoopWorker指针
    if (worker == nullptr || worker->isBusy())
    {
        return true;  // 如果worker为空或繁忙，返回true
    }
    return false;
}

bool AsyncEventLoopInvoker::doInvoker(const std::function<bool()>& callImpl,
    const InvokerNotify& cb, bool blocking)
{
    CallParamIn paramIn;  // 创建空的输入参数

    // 包装函数，将无参数函数转换为带参数的调用
    auto _func = [callImpl](const CallParamIn, CallParamOut& paramOut)->bool
    {
        return callImpl();  // 调用原始的无参数函数
    };

    return doInvoker(_func, paramIn, cb, blocking);  // 调用完整版本
}

bool AsyncEventLoopInvoker::doInvoker(const InvokerCall& callImpl,
    const CallParamIn& paramIn,
    const InvokerNotify& cb, bool blocking)
{
    OBTAIN_OBJ(worker, m_objImpl, EventLoopWorker);

    if (worker == nullptr || worker->isBusy())  // 检查worker状态
    {
        return false;
    }

    // 创建实际执行的任务函数
    auto _func = [paramIn, cb, callImpl]() ->bool
    {
        CallParamOut opResult;  // 输出参数
        bool success = callImpl(paramIn, opResult);  // 调用用户函数

        opResult["call_success"] = success;  // 将执行结果存入输出参数

        if (cb != nullptr)  // 如果有回调函数
        {
            cb(opResult);  // 执行回调
        }
        return success;
    };

    // 创建任务数据对象
    auto taskFunCall = std::make_shared<EventLoopWorker::TaskFunCallData>(_func);

    if (!blocking)  // 非阻塞调用
    {
        emit worker->signal_doTask_noblocking(taskFunCall);  // 发射非阻塞信号
    }
    else  // 阻塞调用
    {
        emit worker->signal_doTask_blocking(taskFunCall);  // 发射阻塞信号
    }

    return true;
}

AsyncEventLoopInvoker::AsyncEventLoopInvoker()
{
    // 构造函数，m_objImpl初始化为nullptr
}

AsyncEventLoopInvoker::~AsyncEventLoopInvoker()
{
    unInit();  // 析构时自动清理
}

void AsyncEventLoopInvoker::init(const ExceptionHandle& exceptionHandle)
{
    OBTAIN_OBJ(worker, m_objImpl, EventLoopWorker);
    if (worker == nullptr)  // 如果worker尚未创建
    {
        worker = new EventLoopWorker();  // 创建新的EventLoopWorker
        worker->init(exceptionHandle);   // 初始化worker

        m_objImpl = worker;  // 保存worker指针
    }
}

void AsyncEventLoopInvoker::unInit()
{
    OBTAIN_OBJ(worker, m_objImpl, EventLoopWorker);
    if (worker != nullptr)
    {
        worker->unInit();  // 停止worker线程
        delete worker;     // 删除worker对象

        m_objImpl = nullptr;  // 重置指针
    }
}

END_COMMON_TOOLS_SPACE