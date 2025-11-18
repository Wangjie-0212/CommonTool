#ifndef OBJPRODUCEFACTORY_H
#define OBJPRODUCEFACTORY_H


#include "commontools_global.h"
#include "CommonToolsDef.h"

BEGIN_COMMON_TOOLS_SPACE

class COMMONTOOLS_EXPORT ObjProduceFactory
{
public:
    // 模板方法：创建指定类型的对象
    template<typename TClass>
    static TClass* createObj(const ExceptionHandle& exceptionHandle)
    {
        TClass* t = NULL;
        QString className = getMsgClassName(t);  // 获取类名（代码不完整）
        // 这里应该调用具体的创建逻辑
    }

    // 根据类名创建对象
    static void* createObj(const QString& className, const ExceptionHandle& exceptionHandle);
};

#endif // !OBJPRODUCEFACTORY_H

END_COMMON_TOOLS_SPACE