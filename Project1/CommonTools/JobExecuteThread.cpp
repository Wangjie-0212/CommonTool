#include "JobExecuteThread.h"

JobExecuteThread::JobExecuteThread(const JobFunc& jobFunc)
	:m_jobFun(jobFunc)
{
}

JobExecuteThread::JobExecuteThread(const JobFunc& jobFunc, const QVariant& param1, const QVariant& param2)
	: m_jobFun(jobFunc)
	, m_param1(param1)
	, m_param2(param2)
{
}

JobExecuteThread::~JobExecuteThread()
{
	stop();
}

bool JobExecuteThread::start()
{
	if (m_started || m_jobFun == nullptr)
	{
		return false;
	}
	
	QThread::start();

	m_started = true;
	return true;
}

bool JobExecuteThread::stop(int32_t waitInterval)
{
	if (!wait(waitInterval))
	{
		terminate();
		return false;
	}

	return false;
}

void JobExecuteThread::run()
{
	//线程执行函数
	if (m_jobFun != nullptr)
	{
		m_jobFun(m_param1,m_param2);
	}
}
