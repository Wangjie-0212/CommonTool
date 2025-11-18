#include "EventLoopWorker.h"
#include "MacroDef.h"

BEGIN_COMMON_TOOLS_SPACE

EventLoopWorker::EventLoopWorker()
{
    m_doWroking.store(false);  // 初始化为非工作状态

    // 注册自定义类型，用于信号槽传递
    COMMONTOOL_Q_DECLARE_METATYPE(PTaskFunCallData);

    // 连接信号和槽
    // 非阻塞连接：任务在目标线程的事件循环中异步执行
    connect(this, &EventLoopWorker::signal_doTask_noblocking,
        this, &EventLoopWorker::on_doTask);

    // 阻塞连接：任务在目标线程同步执行，调用线程会等待
    connect(this, &EventLoopWorker::signal_doTask_blocking,
        this, &EventLoopWorker::on_doTask, Qt::BlockingQueuedConnection);
}

EventLoopWorker::~EventLoopWorker()
{
    // 默认析构函数
}

void EventLoopWorker::init(const ExceptionHandle& exceptionhandle)
{
    if (QThread::isRunning())  // 如果线程已经在运行，直接返回
    {
        return;
    }

    moveToThread(this);  // 将对象移动到自己的线程，确保信号槽在正确线程执行

    m_exceptionHandle = exceptionhandle;  // 设置异常处理回调

    QThread::start();  // 启动线程，开始事件循环
}

void EventLoopWorker::unInit()
{
    if (QThread::isRunning())  // 如果线程在运行
    {
        QThread::quit();  // 请求退出事件循环
        QThread::msleep(200);  // 等待200ms

        if (QThread::wait(5000))  // 等待5秒线程正常退出
        {
            QThread::terminate();  // 如果超时，强制终止线程
        }

        m_exceptionHandle = nullptr;  // 清空异常处理回调
    }
}

bool EventLoopWorker::isBusy()
{
    return m_doWroking.load();  // 返回当前工作状态
}

void EventLoopWorker::on_doTask(PTaskFunCallData taskFunCall)
{
    m_doWroking.store(true);  // 设置为工作状态

    try
    {
        if (taskFunCall != nullptr && taskFunCall->m_taskFun != nullptr)
        {
            taskFunCall->m_taskFun();  // 执行实际的任务函数
        }
    }
    catch (...)  // 捕获所有异常
    {
        m_doWroking.store(false);  // 发生异常，重置工作状态

        if (m_exceptionHandle != nullptr)
        {
            // 调用异常处理回调
            m_exceptionHandle(ExceptionTaskThreadCrash,
                QString("EventLoopWorker execulte tash has a exception!"));
        }
    }
    m_doWroking.store(false);  // 任务完成，重置工作状态
}

END_COMMON_TOOLS_SPACE