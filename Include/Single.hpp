#ifndef __SINGLE_HPP__
#define __SINGLE_HPP__

#include <functional>
#include <mutex>
using std::shared_ptr;

//懒汉式（延迟加载）单例模版
//继承实现方式
//智能指针无法托管私有构造函数的类（继承类的问题），故自制RAII
template<typename T>
class Single
{
public:
	template<typename... Args>
	static T* getInstence(Args&&... args)
	{
		//避免每次调用都加锁
		if (nullptr == _pInstance)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (nullptr == _pInstance)
				_pInstance = new T(std::forward<Args>(args)...);
		}
		return _pInstance;
	}
    Single(const Single&) = delete;
    Single& operator=(const Single&) = delete;

protected:
	//用于自动销毁 RAII
	class AutoRelease
	{
	public:
		AutoRelease() = default;
		~AutoRelease()
		{
			if (nullptr != _pInstance)
				delete _pInstance;
		}
	};

	Single() = default;
	virtual ~Single() = default;
//	Single(const Single&&) = delete;
//	Single& operator=(const Single&&) = delete;

private:
	static T* _pInstance;
	static AutoRelease _autoRelease;
	static std::mutex _mutex;
};

template<typename T>
T* Single<T>::_pInstance = nullptr;

template<typename T>
typename Single<T>::AutoRelease Single<T>::_autoRelease;

template<typename T>
std::mutex Single<T>::_mutex;

#endif // !__SINGLE_HPP__
