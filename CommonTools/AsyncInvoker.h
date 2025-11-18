#ifndef ASYNC_INVOKER_H
#define ASYNC_INVOKER_H

#include "CommonToolsDef.h"
#include "commontools_global.h"

#include <stdint.h>
#include <functional>
#include <memory>
#include <QMap>
#include <QVariant>


class TaskScheduler;

BEGIN_COMMON_TOOLS_SPACE

using CallParamIn  = QMap<QString, QVariant>;
using CallParamOut = QMap<QString, QVariant>;

//异步操作接口回调
using InvokerNotify = std::function<void(const CallParamOut& paramOut)>;
using InvokerCall = std::function<bool(const CallParamIn& paramIn, CallParamOut& paramOut)>;

//无时间循环的任务执行器，【内部有自己的线程，任务在内部线程执行】，外部可通过继承使用，也可通过ObjProduceFactoryInner类创建
class COMMONTOOLS_EXPORT AsyncInvoker
{
    friend class ObjProduceFactoryInner;  // 友元类
public:
    bool isBusy();  // 检查是否繁忙

    // 简化版本调用
    bool doInvoker(const std::function<bool()>& callImpl, const InvokerNotify& cb);

    // 完整版本调用
    bool doInvoker(const InvokerCall& callImpl, const CallParamIn& paramIn,
        const InvokerNotify& cb);

    // 队列方式调用（总是异步）
    bool doInvokerInQueue(const std::function<bool(const CallParamIn& paramIn,
        CallParamOut& paramOut)>& callImpl,
        const CallParamIn& paramIn, const InvokerNotify& cb);

protected:
    AsyncInvoker();   // 保护构造函数
    ~AsyncInvoker();  // 析构函数

protected:
    void init(const ExceptionHandle& exceptionHandle);  // 初始化
    void unInit();  // 反初始化

protected:
    std::shared_ptr<TaskScheduler> m_scheduler;  // 任务调度器
};
END_COMMON_TOOLS_SPACE
#endif // !ASYNC_INVOKER_H
