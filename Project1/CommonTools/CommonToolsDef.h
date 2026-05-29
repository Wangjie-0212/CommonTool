#ifndef COMMONTOOLS_DEF_H
#define COMMONTOOLS_DEF_H

#include "CommonToolsSpace.h"
#include <functional>
#include <QString>
#include <QMetaType>

BEGIN_COMMON_TOOLS_SPACE

enum ExceptionCode
{
	ExceptionNull = 0,

	ExceptionTaskThreadCrash = 1,
};

//异常回调
using ExceptionHandle = std::function<void(int code, const QString& tip)>;

//qt的自定义类型metaType注册
#define CM_Q_DECLARE_METATYPE(TYPE)\
if (!QMetaType::type(#TYPE) || !QMetaType::isRegistered(QMetaType::type(#TYPE) ) )\
{\
    qRegisterMetaType<TYPE>(#TYPE);\
}\

// 匿名函数支持递归定义，常用于使用lambda表达式递归调用
template<typename F>
inline auto PW_Y_FNC(F&& f)
{
	return[f = std::forward<F>(f)](auto&&... args) ->decltype(auto){return f(f, std::forward<decltype(args)>(args)...); };
}

END_COMMON_TOOLS_SPACE

#endif // !COMMONTOOLS_DEF_H

