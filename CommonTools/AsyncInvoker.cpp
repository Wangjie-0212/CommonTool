#include "AsyncInvoker.h"
#include "wrapper_thread.h"

BEGIN_COMMON_TOOLS_SPACE

AsyncInvoker::AsyncInvoker()
{
    // 构造函数，m_scheduler在init中初始化
}

AsyncInvoker::~AsyncInvoker()
{
    unInit();  // 析构时自动清理
}

void AsyncInvoker::init(const ExceptionHandle& exceptionHandle)
{
    if (m_scheduler == nullptr)  // 如果调度器未创建
    {
        m_scheduler = std::make_shared<TaskScheduler>();  // 创建任务调度器
        m_scheduler->setExceptionHandle(exceptionHandle); // 设置异常处理
        m_scheduler->start();  // 启动调度器
    }
}

void AsyncInvoker::unInit()
{
    if (m_scheduler != nullptr)
    {
        m_scheduler->stop();  // 停止调度器
        m_scheduler = nullptr;  // 释放调度器
    }
}

bool AsyncInvoker::isBusy()
{
    if (m_scheduler == nullptr || m_scheduler->isBusy())
    {
        return true;  // 调度器为空或繁忙时返回true
    }
    return false;
}

bool AsyncInvoker::doInvoker(const InvokerCall& callImpl,
    const CallParamIn& paramIn, const InvokerNotify& cb)
{
    if (isBusy())  // 检查是否繁忙
    {
        return false;
    }

    // 创建包装函数
    auto _func = [paramIn, cb, callImpl]() -> bool
    {
        CallParamOut opResult;
        bool success = callImpl(paramIn, opResult);  // 执行用户函数
        opResult["call_success"] = success;  // 存储执行结果

        if (cb != nullptr)
        {
            cb(opResult);  // 执行回调
        }
        return success;
    };

    m_scheduler->asyncExecuteTask(_func);  // 异步执行任务
    return true;
}

bool AsyncInvoker::doInvoker(const std::function<bool()>& callImpl,
    const InvokerNotify& cb)
{
    CallParamIn paramIn;  // 空参数

    // 包装无参数函数
    auto _func = [callImpl](const CallParamIn& paramIn, CallParamOut& paramOut) -> bool
    {
        return callImpl();  // 调用原始函数
    };

    return doInvoker(_func, paramIn, cb);  // 调用完整版本
}

bool AsyncInvoker::doInvokerInQueue(
    const std::function<bool(const CallParamIn& paramIn, CallParamOut& paramOut)>& callImpl,
    const CallParamIn& paramIn, const InvokerNotify& cb)
{
    // 创建任务函数
    auto _func = [paramIn, cb, callImpl]() -> bool
    {
        CallParamOut opResult;
        bool success = callImpl(paramIn, opResult);  // 执行用户函数
        if (cb != nullptr)
        {
            opResult["call_success"] = success;
            cb(opResult);  // 执行回调
        }
        return success;
    };

    m_scheduler->asyncExecuteTask(_func);  // 异步执行（总是进入队列）
    return true;
}

END_COMMON_TOOLS_SPACE
