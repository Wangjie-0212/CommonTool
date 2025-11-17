#ifndef JOBEXECUTETHREAD_H
#define JOBEXECUTETHREAD_H

#include "commontools_global.h"
#include <QObject>
#include <stdint.h>
#include <functional>
#include <QVariant>
#include <QThread>
#include <memory>


//执行的函数体
typedef std::function<bool(const QVariant& p1, const QVariant& p2)> JobFunc;

//任务执行线程
class COMMONTOOLS_EXPORT JobExecuteThread : public QThread
{
public:
	explicit JobExecuteThread(const JobFunc& jobFunc);
	explicit JobExecuteThread(const JobFunc& jobFunc, const QVariant& param1, const QVariant& param2);

	~JobExecuteThread();

	bool start();

	bool stop(int32_t waitInterval = 2000);
protected:
	void run() override;

private:
	//执行函数体
	JobFunc m_jobFun = nullptr;

	//参数1
	QVariant m_param1;

	//参数2
	QVariant m_param2;

	//状态
	bool m_started = false;
};

#endif // !JOBEXECUTETHREAD_H
