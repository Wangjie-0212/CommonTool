#ifndef MACRODEF_H
#define MACRODEF_H

#ifndef COMMONTOOL_Q_DECLARE_METATYPE

#include <QMetaType>

#define COMMONTOOL_Q_DECLARE_METATYPE(TYPE)\
if(!QMetaType::type(#TYPE) || !QMetaType::isRegistered(QMetaType::type(#TYPE)))\
{\
	qRegisterMetaType<TYPE>(#TYPE);\
}\

#endif // !COMMONTOOL_Q_DECLARE_METATYPE

#endif // !MACRODEF_H

