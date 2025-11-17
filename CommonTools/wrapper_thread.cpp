#include "wrapper_thread.h"


WrapperThread::WrapperThread(const ThreadCall& threadCall, void* pData)
	: m_threadCall(threadCall)
	, m_pData(pData)
{
}

WrapperThread::~WrapperThread()
{
}

void WrapperThread::run()
{
	if (m_threadCall)
	{
		m_threadCall(m_pData);
	}
}


TaskScheduler::TaskObj::TaskObj(const TaskFun& fun, bool asyn, TaskScheduler* scheduler)
	:_func(fun)
	, _asyn(asyn)
	, _scheduler(scheduler)
{
}

TaskScheduler::TaskObj::~TaskObj()
{
}

void TaskScheduler::TaskObj::run()
{
	if (_func == nullptr)
	{
		return;
	}

	bool success = _func();

	//异步调用
	if (_asyn)
	{
		return;
	}

	if (_scheduler)
	{
		_scheduler->comitResult(success);
	}
}

TaskScheduler::TaskScheduler()
{
	m_stop.store(true);
	m_courseLock.store(false);
	m_courseResult.store(false);

	m_threadWorking = std::make_shared<SharedBoolParam>();
	m_threadWorking->store(false);
}

TaskScheduler::~TaskScheduler()
{
	stop();
}

bool TaskScheduler::start()
{
	if (m_spThread != nullptr)
	{
		return true;
	}

	m_stop.store(false);
	m_spThread = std::make_shared<std::thread>([this]()
		{
			ThreadFunc();
		});

	return true;
}

void TaskScheduler::stop()
{
	if (m_spThread == nullptr)
	{
		return;
	}

	m_stop.store(true);
	m_cond.notify_one();

	const int32_t mtime = 2 * 1000;
	const int32_t step = 100;

	int32_t tick = mtime / step;
	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(step));

		if (!m_spThread->joinable())
		{
			break;
		}

	} while (--tick > 0);

	if (m_spThread->joinable())
	{
		m_spThread->detach();
	}

	//kill thread
	m_spThread = nullptr;
}

void TaskScheduler::asyncExecuteTask(const TaskFun& fun)
{
	SPTaskObj spTaskObj = std::make_shared<TaskObj>(fun, true, this);
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_taskList.push_back(spTaskObj);
	}

	m_cond.notify_one();
}

bool TaskScheduler::syncExecuteTask(const TaskFun& fun, int32_t timeOutMs)
{
	//保证只有一个任务可执行，从而模拟同步执行
	{
		bool expected = false;
		bool desired = true;
		while (!m_courseLock.compare_exchange_weak(expected, desired))
		{
			auto thread = std::this_thread::get_id();
			int32_t threadId = *(int32_t*)(&thread);
			int32_t delay = 60 + (threadId % 10) * 10;

			std::this_thread::sleep_for(std::chrono::milliseconds(delay));

			expected = false;
		}

		SPSyncInfo spSyncInfo = std::make_shared<syncInfo>();
		SPTaskObj spTaskObj = std::make_shared<TaskObj>(fun, false, this);
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_taskList.push_back(spTaskObj);
		}

		m_cond.notify_one();
	}

	if (!waitForJobEnd())
	{
		return false;
	}

	return m_courseResult.load();
}

void TaskScheduler::cleanTask()
{
	decltype(m_taskList) temp;
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_taskList.swap(temp);
	}
}

bool TaskScheduler::isBusy() const
{
	return m_threadWorking->load();
}

void TaskScheduler::setExceptionHandle(const ExceptionHandle& exceptionHandle)
{
	m_execptionHandle = exceptionHandle;
}

void TaskScheduler::ThreadFunc()
{
	auto exceptionHandle = m_execptionHandle;
	auto threadWorking = m_threadWorking;
	bool needWait = false;
	threadWorking->store(true);

	while (true)
	{
		if (m_stop.load())
		{
			break;
		}

		if (needWait)
		{
			threadWorking->store(false);
			std::unique_lock<std::mutex> lock(m_condMutex);
			m_cond.wait(lock);
			threadWorking->store(true);
		}

		//获取一个task
		SPTaskObj spTaskObj;
		{
			needWait = false;
			std::unique_lock<std::mutex> lock(m_mutex);
			if (!m_taskList.empty())
			{
				spTaskObj = m_taskList.front();
				m_taskList.pop_front();
			}
			else
			{
				needWait = true;
			}
		}

		if (spTaskObj == nullptr)
		{
			continue;
		}

		try
		{
			spTaskObj->run();
		}
		catch (...)
		{
			if (exceptionHandle != nullptr)
			{
				exceptionHandle(ExceptionTaskThreadCrash, QString("TaskScheduler execulte tash has a execption!"));
			}
		}
	}

}

bool TaskScheduler::waitForJobEnd(uint32_t interval)
{
	uint32_t tick = interval / 100;
	do
	{
		if (!m_courseLock.load())
		{
			return true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	} while (--tick > 0);

	return false;
}

void TaskScheduler::comitResult(bool result)
{
	m_courseLock.store(false);
	m_courseResult.store(result);
}
