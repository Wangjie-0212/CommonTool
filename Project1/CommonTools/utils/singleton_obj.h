#ifndef sigleton_obj_h
#define sigleton_obj_h

#include "CommonToolsDef.h"

#include<memory>
#include<xcall_once.h>

BEGIN_COMMON_TOOLS_SPACE

//对象版本
template<typename T>
class Singleton
{
public:
	Singleton( const Singleton&) = delete;
	Singleton& operator = ( const Singleton&) = delete;

	//
	static T& instance()
	{
		static T s_instance;
		return instance;
	}
protected:
	Singleton() = default;
	~Singleton() = default;

};
template<typename T>
class SingletonObj
{
public:
	SingletonObj() = delete;
	~SingletonObj() = delete;

	template<typename... Args>
	static T* InitFromArgs(Args&&... args)
	{
		std::call_once(s_initflag, [&]())
		{
			s_instance.reset(new T(std::forward<Args>(args)...));
		});
		return s_instance.get();
	}

	//需要校验返回值T* 与obj是否为同一值，若不为同一值需要手动 obj
	static T* init(T* obj)
	{
		std::call_once(s_initflag, [&]()
			{
				s_instance.reset();
			});
		return s_instance.get();
	}

	//非线程安全
	static void uninit()
	{
		if (s_instance == nullptr)
		{
			return;
		}

		s_instance.reset(nullptr);
	}

	static T* instance()
	{
		std::call_once(s_initflag, []()
			{
				s_instance.reset(new T());
			});
		return s_instance.get();
	}

	static T* obtainInst()
	{
		return s_instance.get();
	}


private:
	static std::unique_ptr<T> s_instance;
	static std::once_flag s_initflag;
};

template<typename T>
std::unique_ptr<T> SingletonObj<T>::s_instance;

template<typename T>
std::once_flag SingletonObj<T>::s_initflag;

END_COMMON_TOOLS_SPACE

template<typename T>
using CMSingletonObj = COMMON_TOOLS_SPACE::SingletonObj<T>;
#endif // !sigleton_obj_h
