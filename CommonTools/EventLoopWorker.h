#ifndef EVENTLOOPWORKER_H
#define EVENTLOOPWORKER_H

#include "CommonToolsDef.h"
#include <functional>
#include <QThread>
#include <atomic>

BEGIN_COMMON_TOOLS_SPACE

class EventLoopWorker : public QThread
{
    Q_OBJECT  // Qt元对象系统宏，支持信号槽机制
public:
    using TaskFun = std::function<bool()>;  // 定义任务函数类型

    class TaskFunCallData  // 任务调用数据封装类
    {
    public:
        TaskFunCallData(const TaskFun& taskFun) : m_taskFun(taskFun) {};  // 构造函数
        ~TaskFunCallData() = default;  // 默认析构函数

        TaskFun m_taskFun;  // 实际的任务函数
    };

    using PTaskFunCallData = std::shared_ptr<TaskFunCallData>;  // 任务数据智能指针

    EventLoopWorker();
    ~EventLoopWorker();

public:
    void init(const ExceptionHandle& exceptionhandle);  // 初始化工作线程
    void unInit();  // 反初始化，停止线程

    bool isBusy();  // 检查是否正在执行任务

signals:
    // 非阻塞方式执行任务的信号
    void signal_doTask_noblocking(PTaskFunCallData taskFunCall);
    // 阻塞方式执行任务的信号  
    void signal_doTask_blocking(PTaskFunCallData taskFunCall);

private slots:
    // 执行任务的实际槽函数
    void on_doTask(PTaskFunCallData taskFunCall);

private:
    std::atomic<bool> m_doWroking;  // 原子标志，表示是否正在工作

    ExceptionHandle m_exceptionHandle = nullptr;  // 异常处理回调
};

END_COMMON_TOOLS_SPACE
#endif // !EVENTLOOPWORKER_H