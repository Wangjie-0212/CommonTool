#include "wrapper_thread.h"


WrapperThread::WrapperThread(const ThreadCall& threadCall, void* pData)
	: m_threadCall(threadCall)  // 初始化线程执行函数
	, m_pData(pData)           // 初始化用户数据
{
}

WrapperThread::~WrapperThread()
{
}

void WrapperThread::run()
{
	if (m_threadCall)  // 检查线程函数是否有效
	{
		m_threadCall(m_pData);  // 执行用户定义的线程函数
	}
}


TaskScheduler::TaskObj::TaskObj(const TaskFun& fun, bool asyn, TaskScheduler* scheduler)
	:_func(fun)         // 初始化任务函数
	, _asyn(asyn)       // 初始化异步标志
	, _scheduler(scheduler)  // 初始化调度器指针
{
}

TaskScheduler::TaskObj::~TaskObj()
{
}

void TaskScheduler::TaskObj::run()
{
	if (_func == nullptr)  // 检查任务函数是否有效
	{
		return;
	}

	bool success = _func();  // 执行任务函数

	if (_asyn)  // 如果是异步调用，直接返回
	{
		return;
	}

	if (_scheduler)  // 如果是同步调用，提交结果给调度器
	{
		_scheduler->comitResult(success);
	}
}

TaskScheduler::TaskScheduler()
{
	m_stop.store(true);         // 初始化为停止状态
	m_courseLock.store(false);  // 同步锁初始为未锁定
	m_courseResult.store(false); // 结果初始为false

	// 创建线程工作状态标志，初始为不工作
	m_threadWorking = std::make_shared<SharedBoolParam>();
	m_threadWorking->store(false);
}

TaskScheduler::~TaskScheduler()
{
	stop();  // 析构时自动停止调度器
}

bool TaskScheduler::start()
{
	if (m_spThread != nullptr)  // 如果线程已启动，直接返回
	{
		return true;
	}

	m_stop.store(false);  // 设置运行标志
	// 创建并启动工作线程，绑定ThreadFunc成员函数
	m_spThread = std::make_shared<std::thread>([this]()
		{
			ThreadFunc();
		});

	return true;
}

void TaskScheduler::stop()
{
	if (m_spThread == nullptr)  // 如果线程未启动，直接返回
	{
		return;
	}

	m_stop.store(true);   // 设置停止标志
	m_cond.notify_one();  // 通知工作线程退出等待

	// 等待线程结束的超时机制
	const int32_t mtime = 2 * 1000;  // 总等待时间2秒
	const int32_t step = 100;        // 每次检查间隔100ms
	int32_t tick = mtime / step;     // 总检查次数

	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(step));

		if (!m_spThread->joinable())  // 检查线程是否可join
		{
			break;
		}

	} while (--tick > 0);

	if (m_spThread->joinable())  // 如果超时后线程仍未结束，detach它
	{
		m_spThread->detach();
	}

	m_spThread = nullptr;  // 释放线程指针
}

void TaskScheduler::asyncExecuteTask(const TaskFun& fun)
{
	// 创建异步任务对象
	SPTaskObj spTaskObj = std::make_shared<TaskObj>(fun, true, this);
	{
		std::unique_lock<std::mutex> lock(m_mutex);  // 锁定任务队列
		m_taskList.push_back(spTaskObj);             // 添加任务到队列
	}

	m_cond.notify_one();  // 通知工作线程有新任务
}

bool TaskScheduler::syncExecuteTask(const TaskFun& fun, int32_t timeOutMs)
{
	//保证只有一个任务可执行，从而模拟同步执行
	{
		// 使用原子操作确保同一时间只有一个同步任务在执行
		bool expected = false;
		bool desired = true;

		// CAS循环等待同步锁释放
		while (!m_courseLock.compare_exchange_weak(expected, desired))
		{
			// 计算随机退避时间，避免活锁
			auto thread = std::this_thread::get_id();
			int32_t threadId = *(int32_t*)(&thread);
			int32_t delay = 60 + (threadId % 10) * 10;

			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
			expected = false;  // 重置期望值
		}

		// 创建同步任务
		SPSyncInfo spSyncInfo = std::make_shared<syncInfo>();
		SPTaskObj spTaskObj = std::make_shared<TaskObj>(fun, false, this);
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_taskList.push_back(spTaskObj);
		}

		m_cond.notify_one();  // 通知工作线程
	}

	// 等待任务完成
	if (!waitForJobEnd(timeOutMs))
	{
		return false;
	}

	return m_courseResult.load();  // 返回任务执行结果
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
	auto exceptionHandle = m_execptionHandle;  // 保存异常处理回调
	auto threadWorking = m_threadWorking;      // 保存工作状态标志
	bool needWait = false;                     // 是否需要等待新任务
	threadWorking->store(true);                // 设置工作状态为true

	while (true)
	{
		if (m_stop.load())  // 检查停止标志
		{
			break;
		}

		if (needWait)  // 如果没有任务，进入等待状态
		{
			threadWorking->store(false);  // 设置非工作状态
			std::unique_lock<std::mutex> lock(m_condMutex);
			m_cond.wait(lock);            // 等待条件变量通知
			threadWorking->store(true);   // 被唤醒后设置工作状态
		}

		// 从任务队列获取任务
		SPTaskObj spTaskObj;
		{
			needWait = false;
			std::unique_lock<std::mutex> lock(m_mutex);
			if (!m_taskList.empty())
			{
				spTaskObj = m_taskList.front();  // 获取队列首任务
				m_taskList.pop_front();          // 从队列移除
			}
			else
			{
				needWait = true;  // 队列为空，需要等待
			}
		}

		if (spTaskObj == nullptr)  // 检查任务是否有效
		{
			continue;
		}

		try
		{
			spTaskObj->run();  // 执行任务
		}
		catch (...)  // 捕获所有异常
		{
			if (exceptionHandle != nullptr)
			{
				// 调用异常处理回调
				exceptionHandle(ExceptionTaskThreadCrash,
					QString("TaskScheduler execulte tash has a execption!"));
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
