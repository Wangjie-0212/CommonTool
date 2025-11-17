#ifndef COMMONTOOLS_GLOBAL_H
#define COMMONTOOLS_GLOBAL_H


#include <QtCore/qglobal.h>

# if defined(COMMONTOOLS_LIB)
#  define COMMONTOOLS_EXPORT Q_DECL_EXPORT
# else
#  define COMMONTOOLS_EXPORT Q_DECL_IMPORT
# endif
#else

#endif // !COMMONTOOLS_GLOBAL_H
