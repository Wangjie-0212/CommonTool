#include "AsyncInvoker.h"
#include "wrapper_thread.h"

BEGIN_COMMON_TOOLS_SPACE

AsyncInvoker::AsyncInvoker()
{
}
AsyncInvoker::~AsyncInvoker()
{
    unInit();
}
void AsyncInvoker::init(const ExceptionHandle& exceptionHandle)
{
    if (m_scheduler == nullptr)
    {
        m_scheduler = std::make_shared<TaskScheduler>();
        m_scheduler->setExceptionHandle(exceptionHandle);
        m_scheduler->start();
    }
}
void AsyncInvoker::unInit()
{
    if (m_scheduler != nullptr)
    {
        m_scheduler->stop();
        m_scheduler = nullptr;
    }
}

bool AsyncInvoker::isBusy()
{
    if (m_scheduler == nullptr || m_scheduler->isBusy())
    {
        return true;
    }
    return false;
}

bool AsyncInvoker::doInvoker(const InvokerCall& callImpl, const CallParamIn& paramIn, const InvokerNotify& cb)
{
    if (isBusy())
    {
        return false;
    }

    auto _func = [paramIn, cb, callImpl]() -> bool
    {
        CallParamOut opResult;
        bool success = callImpl(paramIn, opResult);
        opResult["call_success"] = success;

        if (cb != nullptr)
        {
            cb(opResult);
        }
        return  success;
    };

    m_scheduler->asyncExecuteTask(_func);
    return true;
}

bool AsyncInvoker::doInvoker(const std::function<bool()>& callImpl, const InvokerNotify& cb)
{
    CallParamIn paramIn;

    auto _func = [callImpl](const CallParamIn& paramIn, CallParamOut& paramOut) -> bool
    {
        return callImpl();
    };

    return doInvoker(_func, paramIn, cb);
}

bool AsyncInvoker::doInvokerInQueue(const std::function<bool(const CallParamIn& paramIn, CallParamOut& paramOut)>& callImpl, const CallParamIn& paramIn, const InvokerNotify& cb)
{
    auto _func = [paramIn, cb, callImpl]() -> bool
    {
        CallParamOut opResult;
        bool success = callImpl(paramIn, opResult);
        if (cb != nullptr)
        {
            opResult["call_success"] = success;
            cb(opResult);
        }

        return success;
    };
    m_scheduler->asyncExecuteTask(_func);
    return true;
}

END_COMMON_TOOLS_SPACE
