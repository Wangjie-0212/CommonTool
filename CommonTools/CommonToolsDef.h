#ifndef COMMONTOOLS_DEF_H
#define COMMONTOOLS_DEF_H

#include "CommonToolsSpace.h"  // 包含命名空间定义
#include <functional>  // 包含std::function
#include <QString>     // 包含Qt字符串类

BEGIN_COMMON_TOOLS_SPACE  // 开始CommonTools命名空间

enum ExceptionCode
{
    ExceptionNull = 0,  // 无异常

    ExceptionTaskThreadCrash = 1,  // 任务线程崩溃异常
};

// 异常回调函数类型定义：参数为异常码和提示信息
using ExceptionHandle = std::function<void(int code, const QString& tip)>;

END_COMMON_TOOLS_SPACE  // 结束命名空间

#endif // !COMMONTOOLS_DEF_H