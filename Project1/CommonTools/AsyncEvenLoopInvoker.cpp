#include "AsyncEvenLoopInvoker.h"
#include "EventLoopWorker.h"

#include <QThread>
#include <QObject>
#include <atomic>

BEGIN_COMMON_TOOLS_SPACE

#define OBTAIN_OBJ(worker, objImpl, TypeName); auto worker = (TypeName*)objImpl;

bool AsyncEventLoopInvoker::isBusy()
{
	OBTAIN_OBJ(worker, m_objImpl, EventLoopWorker);
	if (worker == nullptr || worker->isBusy())
	{
		return true;
	}
	return false;
}

bool AsyncEventLoopInvoker::doInvoker(const std::function<bool()>& callImpl, const InvokerNotify& cb, bool blocking /*= false*/)
{
	CallParamIn paramIn;

	auto _func = [callImpl](const CallParamIn, CallParamOut& paramOut)->bool
	{
		return callImpl();
	};	

	return doInvoker(_func, paramIn, cb, blocking);
}

bool AsyncEventLoopInvoker::doInvoker(const InvokerCall& callImpl, const CallParamIn& paramIn, const InvokerNotify& cb, bool blocking /*= false*/)
{
	OBTAIN_OBJ(worker, m_objImpl, EventLoopWorker);

	if (worker == nullptr || worker->isBusy())
	{
		return false;
	}

	auto _func = [paramIn, cb, callImpl]() ->bool
	{
		CallParamOut opResult;
		bool success = callImpl(paramIn, opResult);

		opResult["call_success"] = success;

		if (cb != nullptr)
		{
			cb(opResult);
		}
		return success;
	};
	auto taskFunCall = std::make_shared<EventLoopWorker::TaskFunCallData>(_func);

	if (!blocking)
	{
		emit worker->signal_doTask_noblocking(taskFunCall);
	}
	else
	{
		emit worker->signal_doTask_blocking(taskFunCall);
	}

	return true;
}

AsyncEventLoopInvoker::AsyncEventLoopInvoker()
{
}

AsyncEventLoopInvoker::~AsyncEventLoopInvoker()
{
	unInit();
}

void AsyncEventLoopInvoker::init(const ExceptionHandle& exceptionHandle)
{
	OBTAIN_OBJ(worker, m_objImpl, EventLoopWorker);
	if (worker == nullptr)
	{
		worker = new EventLoopWorker();
		worker->init(exceptionHandle);

		m_objImpl = worker;
	}
}

void AsyncEventLoopInvoker::unInit()
{
	OBTAIN_OBJ(worker, m_objImpl, EventLoopWorker);
	if (worker != nullptr)
	{
		worker->unInit();
		delete worker;

		m_objImpl = nullptr;
	}
}

END_COMMON_TOOLS_SPACE