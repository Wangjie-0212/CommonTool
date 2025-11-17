#include "ObjProduceFactory.h"
#include "AsyncInvoker.h"
#include "AsyncEvenLoopInvoker.h"

BEGIN_COMMON_TOOLS_SPACE

#define _CREATE_FUNC(TClass)\
static TClass* _createFunc_##TClass(const ExceptionHandle& exceptionHandle)\
{\
	TClass* obj = new TClass();\
	obj->init(exceptionHandle);\
	return obj;\
}\

#define BEGIN_REG_FUNC_CALL QMap<QString, FuncCall> s_funcMap ={
#define END_REG_FUNC_CALL };
#define REG_FUNC_CALL(TClass) {#TClass, std::bind(&ObjProduceFactoryInner::_createFunc_##TClass, std::placeholders::_1)},

#define OBTAIN_FUNC_CALL(func, className) auto func = s_funcMap.value(className);

class ObjProduceFactoryInner 
{
public:
	//创建方法
	_CREATE_FUNC(AsyncInvoker)
    _CREATE_FUNC(AsyncEventLoopInvoker)
};

namespace
{
	using FuncCall = std::function<void* (const ExceptionHandle& exceprionHandle)>;

	//注册方法
	BEGIN_REG_FUNC_CALL
		REG_FUNC_CALL(AsyncInvoker)
		REG_FUNC_CALL(AsyncEventLoopInvoker)
	END_REG_FUNC_CALL
}

void* ObjProduceFactory::createObj(const QString& className, const ExceptionHandle& exceptionHandle)
{
	OBTAIN_FUNC_CALL(func, className);
	if (func == nullptr)
	{
		return false;
	}
	return func(exceptionHandle);
}

END_COMMON_TOOLS_SPACE