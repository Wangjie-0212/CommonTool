#ifndef COMMONTOOLSSPACE_H
#define COMMONTOOLSSPACE_H

// 命名空间定义
#ifndef COMMON_TOOLS_SPACE
#define COMMON_TOOLS_SPACE CommonTools  // 定义命名空间名称为CommonTools
#define BEGIN_COMMON_TOOLS_SPACE namespace COMMON_TOOLS_SPACE{  // 开始命名空间宏
#define END_COMMON_TOOLS_SPACE }  // 结束命名空间宏
#define USE_COMMON_TOOLS_SPACE using namespace COMMON_TOOLS_SPACE;  // 使用命名空间宏
#endif // !COMMON_TOOLS_SPACE

#endif // !COMMONTOOLSSPACE_H