#ifndef EVENTLOOPWORKER_H
#define EVENTLOOPWORKER_H

#include "CommonToolsDef.h"
#include <functional>
#include <QThread>
#include <atomic>

BEGIN_COMMON_TOOLS_SPACE

class EventLoopWorker : public QThread
{
	Q_OBJECT
public:
	using TaskFun = std::function<bool()>;

	class TaskFunCallData
	{
	public:
		TaskFunCallData(const TaskFun& taskFun) : m_taskFun(taskFun) {};
		~TaskFunCallData() = default;

		TaskFun m_taskFun;
	};

	using PTaskFunCallData = std::shared_ptr<TaskFunCallData>;

	EventLoopWorker();
	~EventLoopWorker();

public:
	void init(const ExceptionHandle& exceptionhandle);
	void unInit();

	bool isBusy();
signals:
	void signal_doTask_noblocking(PTaskFunCallData taskFunCall);
	void signal_doTask_blocking(PTaskFunCallData taskFunCall);

private:
	void on_doTask(PTaskFunCallData taskFunCall);

private:
	std::atomic<bool> m_doWroking;

	ExceptionHandle m_exceptionHandle = nullptr;
};

END_COMMON_TOOLS_SPACE
#endif // !EVENTLOOPWORKER_H