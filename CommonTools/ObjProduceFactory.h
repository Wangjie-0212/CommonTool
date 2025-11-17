#ifndef OBJPRODUCEFACTORY_H
#define OBJPRODUCEFACTORY_H


#include "commontools_global.h"
#include "CommonToolsDef.h"

BEGIN_COMMON_TOOLS_SPACE

class COMMONTOOLS_EXPORT ObjProduceFactory
{
public:
    //创建一个对象，返回对象指针，生命周期有外部托管
    template<typename TClass>
    static TClass* createObj(const ExceptionHandle& exceptionHandle)
    {
        TClass* t = NULL;
        QString className = getMsgClassName(t);
    }

    static void* createObj(const QString& className, const ExceptionHandle& exceptionHandle);
};

#endif // !OBJPRODUCEFACTORY_H

END_COMMON_TOOLS_SPACE