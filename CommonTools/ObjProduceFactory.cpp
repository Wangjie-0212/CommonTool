#include "ObjProduceFactory.h"
#include "AsyncInvoker.h"
#include "AsyncEvenLoopInvoker.h"

BEGIN_COMMON_TOOLS_SPACE

// 宏定义：为每个类生成创建函数
#define _CREATE_FUNC(TClass)\
static TClass* _createFunc_##TClass(const ExceptionHandle& exceptionHandle)\
{\
	TClass* obj = new TClass();\
	obj->init(exceptionHandle);\
	return obj;\
}\

// 宏定义：开始注册函数映射表
#define BEGIN_REG_FUNC_CALL QMap<QString, FuncCall> s_funcMap ={
#define END_REG_FUNC_CALL };
// 宏定义：注册一个类的创建函数
#define REG_FUNC_CALL(TClass) {#TClass, std::bind(&ObjProduceFactoryInner::_createFunc_##TClass, std::placeholders::_1)},
// 宏定义：从映射表中获取创建函数
#define OBTAIN_FUNC_CALL(func, className) auto func = s_funcMap.value(className);

// 内部工厂实现类
class ObjProduceFactoryInner 
{
public:
	// 为每个支持的类生成创建函数
	_CREATE_FUNC(AsyncInvoker)
    _CREATE_FUNC(AsyncEventLoopInvoker)
};

namespace
{
	using FuncCall = std::function<void* (const ExceptionHandle& exceprionHandle)>;

	// 创建全局的函数映射表，注册所有支持的类
	BEGIN_REG_FUNC_CALL
		REG_FUNC_CALL(AsyncInvoker)
		REG_FUNC_CALL(AsyncEventLoopInvoker)
	END_REG_FUNC_CALL
}

void* ObjProduceFactory::createObj(const QString& className, const ExceptionHandle& exceptionHandle)
{
	OBTAIN_FUNC_CALL(func, className); // 根据类名获取创建函数
	if (func == nullptr)
	{
		return nullptr;
	}
	return func(exceptionHandle); // 调用创建函数并返回对象
}

END_COMMON_TOOLS_SPACE