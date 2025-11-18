#ifndef ASYNC_EVENTLOOP_INVOKER_H
#define ASYNC_EVENTLOOP_INVOKER_H

#include "CommonToolsDef.h"
#include "commontools_global.h"

#include <functional>
#include <QMap>
#include <QVariant>

BEGIN_COMMON_TOOLS_SPACE

using CallParamIn = QMap<QString, QVariant>;
using CallParamOut = QMap<QString, QVariant>;

//异步操作接口回调
using InvokerNotify = std::function<void(const CallParamOut& paramOut)>;
using InvokerCall = std::function<bool(const CallParamIn& paramIn, CallParamOut& paramOut)>;

//无时间循环的任务执行器，【内部有自己的线程，任务在内部线程执行】，外部可通过继承使用，也可通过ObjProduceFactoryInner类创建
class COMMONTOOLS_EXPORT AsyncEventLoopInvoker
{
    friend class ObjProduceFactoryInner;  // 友元类，允许工厂访问protected成员
public:
    bool isBusy();  // 检查是否繁忙

    // 简化版本调用：直接传入函数和回调
    bool doInvoker(const std::function<bool()>& callImpl,
        const InvokerNotify& cb, bool blocking = false);

    // 完整版本调用：支持参数传递
    bool doInvoker(const InvokerCall& callImpl, const CallParamIn& paramIn,
        const InvokerNotify& cb, bool blocking = false);

protected:
    AsyncEventLoopInvoker();   // 保护构造函数，只能通过工厂创建
    ~AsyncEventLoopInvoker();  // 析构函数

protected:
    void init(const ExceptionHandle& exceptionHandle);  // 初始化
    void unInit();  // 反初始化

private:
    void* m_objImpl = nullptr;  // 指向实际EventLoopWorker对象的指针
};
END_COMMON_TOOLS_SPACE
#endif // !ASYNC_EVENTLOOP_INVOKER_H


