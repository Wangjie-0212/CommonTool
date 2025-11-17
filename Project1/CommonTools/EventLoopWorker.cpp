#include "EventLoopWorker.h"
#include "MacroDef.h"

BEGIN_COMMON_TOOLS_SPACE

EventLoopWorker::EventLoopWorker()
{
	m_doWroking.store(false);
	COMMONTOOL_Q_DECLARE_METATYPE(PTaskFunCallData);

	connect(this, &EventLoopWorker::signal_doTask_noblocking, this, &EventLoopWorker::on_doTask);
	connect(this, &EventLoopWorker::signal_doTask_blocking, this, &EventLoopWorker::on_doTask, Qt::BlockingQueuedConnection);
}

EventLoopWorker::~EventLoopWorker()
{
}

void EventLoopWorker::init(const ExceptionHandle& exceptionhandle)
{
	if (QThread::isRunning())
	{
		return;
	}

	moveToThread(this);

	m_exceptionHandle = exceptionhandle;
	//启动线程
	QThread::start();
}

void EventLoopWorker::unInit()
{
	if (QThread::isRunning())
	{
		QThread::quit();
		QThread::msleep(200);

		if (QThread::wait(5000))
		{
			QThread::terminate();
		}

		m_exceptionHandle = nullptr;
	}
}

bool EventLoopWorker::isBusy()
{
	return m_doWroking.load();
}

void EventLoopWorker::on_doTask(PTaskFunCallData taskFunCall)
{
	m_doWroking.store(true);

	try
	{
		if (taskFunCall != nullptr && taskFunCall->m_taskFun != nullptr)
		{
			taskFunCall->m_taskFun();
		}
	}
	catch (...)
	{
		m_doWroking.store(false);

		if (m_exceptionHandle != nullptr)
		{
			m_exceptionHandle(ExceptionTaskThreadCrash, QString("EventLoopWorker execulte tash has a exception!"));
		}
	}
	m_doWroking.store(false);

}

END_COMMON_TOOLS_SPACE