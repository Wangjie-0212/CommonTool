#ifndef COMMONTOOLS_DEF_H
#define COMMONTOOLS_DEF_H

#include "CommonToolsSpace.h"
#include <functional>
#include <QString>

BEGIN_COMMON_TOOLS_SPACE

enum ExceptionCode
{
	ExceptionNull = 0,

	ExceptionTaskThreadCrash = 1,
};

//异常回调
using ExceptionHandle = std::function<void(int code, const QString& tip)>;

END_COMMON_TOOLS_SPACE

#endif // !COMMONTOOLS_DEF_H

