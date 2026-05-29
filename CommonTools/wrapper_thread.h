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
public:
	using ThreadCall = std::function<void(void* pData)>;  // 定义线程执行函数类型

	explicit WrapperThread(const ThreadCall& threadCall, void* pData);  // 显式构造函数
	~WrapperThread();  // 析构函数

protected:
	void run() override;  // 重写QThread的run方法

	ThreadCall m_threadCall = nullptr;  // 线程执行函数
	void* m_pData = nullptr;  // 传递给线程函数的用户数据
};

typedef std::function<bool()> TaskFun;


//任务执行器，通过是std::thread 实现的带任务队列的生产、消费者模型线程（单线程模型）

class COMMONTOOLS_EXPORT TaskScheduler
{
    struct syncInfo  // 同步信息结构体，用于同步调用
    {
        std::condition_variable m_cond;  // 条件变量，用于线程同步等待
        bool _result = false;  // 任务执行结果
    };

    using SPSyncInfo = std::shared_ptr<syncInfo>;  // 同步信息的智能指针
    using WPSyncInfo = std::weak_ptr<syncInfo>;    // 弱指针，避免循环引用

    class TaskObj  // 任务对象封装类
    {
    public:
        explicit TaskObj(const TaskFun& fun, bool asyn, TaskScheduler* scheduler);
        ~TaskObj();
        void run();  // 执行任务

        TaskFun _func;           // 任务函数
        bool _asyn = false;      // 是否异步执行
        TaskScheduler* _scheduler = nullptr;  // 所属调度器
    };

    using SPTaskObj = std::shared_ptr<TaskObj>;  // 任务对象智能指针

public:
    TaskScheduler();
    ~TaskScheduler();

    // 启动任务调度器
    bool start();

    // 停止任务调度器
    void stop();

    // 异步执行任务 - 立即返回，不等待任务完成
    void asyncExecuteTask(const TaskFun& fun);

    // 同步执行任务 - 阻塞等待任务完成，可设置超时
    bool syncExecuteTask(const TaskFun& fun, int32_t timeOutMs = -1);

    // 清空未执行的任务队列
    void cleanTask();

    // 检查调度器是否正在执行任务
    bool isBusy() const;

    // 设置异常处理回调
    void setExceptionHandle(const ExceptionHandle& exceptionHandle);

private:
    // 工作线程执行函数
    void ThreadFunc();

    // 等待任务完成（内部使用）
    bool waitForJobEnd(uint32_t interval = UINT32_MAX);

    // 提交任务执行结果（内部使用）
    void comitResult(bool result);

private:
    std::list<SPTaskObj> m_taskList;        // 任务队列
    std::mutex m_mutex;                     // 任务队列互斥锁
    std::condition_variable m_cond;         // 条件变量，用于线程间通信
    std::mutex m_condMutex;                 // 条件变量专用的互斥锁
    std::shared_ptr<std::thread> m_spThread; // 工作线程

    // 原子操作标志位
    std::atomic_bool m_stop;                // 停止标志
    std::atomic<bool> m_courseLock;         // 同步调用锁
    std::atomic<bool> m_courseResult;       // 同步调用结果

    // 线程工作状态标志
    using SharedBoolParam = std::atomic<bool>;
    std::shared_ptr<SharedBoolParam> m_threadWorking;

    // 异常处理回调
    ExceptionHandle m_execptionHandle = nullptr;
};
#endif // !WRAPPER_THREAD_H
