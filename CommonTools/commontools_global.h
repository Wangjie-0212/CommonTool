#ifndef COMMONTOOLS_GLOBAL_H
#define COMMONTOOLS_GLOBAL_H

#include <QtCore/qglobal.h>  // 包含Qt的全局定义，用于DLL导出/导入

// 如果定义了COMMONTOOLS_LIB，表示正在构建库，使用导出宏
# if defined(COMMONTOOLS_LIB)
#  define COMMONTOOLS_EXPORT Q_DECL_EXPORT
# else
#  define COMMONTOOLS_EXPORT Q_DECL_IMPORT  // 否则使用导入宏
# endif
#else

#endif // !COMMONTOOLS_GLOBAL_H