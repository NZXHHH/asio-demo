#pragma once
#include <memory>
#include <mutex>
#include <iostream>
using namespace std;
template <typename T>
class Singleton
{
	friend class LogicSystem;
	friend class AsioIOServicePool;
private:
	Singleton() = default;
	Singleton<T>& operator=(const Singleton<T>& s) = delete;
	Singleton(const Singleton<T>&) = delete;
	static std::shared_ptr<T> _instance;
public:
	static std::shared_ptr<T> GetInstance()
	{
		static std::once_flag s_flag;
		std::call_once(s_flag, [&]()
			{
				_instance = std::shared_ptr<T>(new T);
			});
		return _instance;
	}
};
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;