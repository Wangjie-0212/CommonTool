#ifndef WRAPPER_THREAD_H
#define WRAPPER_THREAD_H

#include "commontools_global.h"
#include "CommonToolsDef.h"

#include <QThread>
#include <functional>
#include <atomic>

USE_COMMON_TOOLS_SPACE

//Qt QThread封装，支持传入表达式执行线程函数
class COMMONTOOLS_EXPORT WrapperThread : public QThread
{
public :
	using ThreadCall = std::function<void(void* pData)>;

	explicit WrapperThread(const ThreadCall& threadCall, void* pData);
	~WrapperThread();

protected:
	void run() override;

	ThreadCall m_threadCall = nullptr;
	void* m_pData = nullptr;
};

typedef std::function<bool()> TaskFun;


//任务执行器，通过是std::thread 实现的带任务队列的生产、消费者模型线程（单线程模型）

class COMMONTOOLS_EXPORT TaskScheduler
{
	struct syncInfo
	{
		//同步等待锁
		// std::mutex m_mutex;

		//同步等待
		std::condition_variable m_cond;

		//返回值
		bool _result = false;

	};

	using SPSyncInfo = std::shared_ptr<syncInfo>;
	using WPSyncInfo = std::weak_ptr<syncInfo>;

	class TaskObj
	{
	public:
		explicit TaskObj(const TaskFun& fun, bool asyn, TaskScheduler* scheduler);
		~TaskObj();

		void run();

		TaskFun _func;
		bool _asyn = false;
		TaskScheduler* _scheduler = nullptr;

	};

	using SPTaskObj = std::shared_ptr<TaskObj>;

public:
	TaskScheduler();
	~TaskScheduler();


	//启动
	bool start();

	//停止
	void  stop();

	//添加执行任务
	void asyncExecuteTask(const TaskFun& fun);

	//同步调用
	bool syncExecuteTask(const TaskFun& fun, int32_t timeOutMs = -1);

	//清空任务
	void cleanTask();

	//是否繁忙
	bool isBusy() const;

	//设置异常回调
	void setExceptionHandle(const ExceptionHandle& exceptionHandle);

private:
	//线程执行函数
	void ThreadFunc();

private:
	//同步等待jobEnd
	bool waitForJobEnd(uint32_t interval = UINT32_MAX);

	//提交结果
	void comitResult(bool result);

private:

	//任务对象队列
	std::list<SPTaskObj> m_taskList;

	//任务对象锁
	std::mutex m_mutex;

	//调度事件
	std::condition_variable m_cond;

	//事件激活锁
	std::mutex m_condMutex;

	//调用线程
	std::shared_ptr<std::thread> m_spThread;

	//线程状态标志位
	std::atomic_bool m_stop;

	//线程同步变量
	std::atomic<bool> m_courseLock;
	std::atomic<bool> m_courseResult;

	//线程是否工作中
	//std::atomic<bool> m_threadWorking;

	using SharedBoolParam = std::atomic<bool>;
	std::shared_ptr<SharedBoolParam> m_threadWorking;

	//异常处理
	ExceptionHandle m_execptionHandle = nullptr;
};

#endif // !WRAPPER_THREAD_H
