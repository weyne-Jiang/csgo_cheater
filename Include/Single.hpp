#ifndef __SINGLE_HPP__
#define __SINGLE_HPP__

#include <functional>
#include <mutex>
using std::shared_ptr;

//����ʽ���ӳټ��أ�����ģ��
//�ɴ���
template<typename T>
class Single_T
{
public:
	template<typename... Args>
	static shared_ptr<T> getInstence(Args&&... args)
	{
		//����ÿ�ε��ö�����
		if (nullptr == _pInstance)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (nullptr == _pInstance)
				_pInstance = std::make_shared<T>(std::forward<Args>(args)...);
		}

		return _pInstance;
	}
	
protected:
	Single_T() = default;
	~Single_T() = default;
	Single_T(const Single_T&) = delete;
	Single_T& operator=(const Single_T&) = delete;
	Single_T(const Single_T&&) = delete;
	Single_T& operator=(const Single_T&&) = delete;

private:
	static shared_ptr<T> _pInstance;
	static std::mutex _mutex;
};

template<typename T>
shared_ptr<T> Single_T<T>::_pInstance = nullptr;

template<typename T>
std::mutex Single_T<T>::_mutex;

//�̳�ʵ�ַ�ʽ
//����ָ���޷��й�˽�й��캯�����ࣨ�̳�������⣩��������RAII
template<typename T>
class Single
{
public:
	template<typename... Args>
	static T* getInstence(Args&&... args)
	{
		//����ÿ�ε��ö�����
		if (nullptr == _pInstance)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (nullptr == _pInstance)
				_pInstance = new T(std::forward<Args>(args)...);
		}
		return _pInstance;
	}

protected:
	//�����Զ����� RAII
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

	Single()
	{
		cout << "Single()\n";
	}
	virtual ~Single()
	{
		cout << "~Single()\n";
	}
	Single(const Single&) = delete;
	Single& operator=(const Single&) = delete;
	Single(const Single&&) = delete;
	Single& operator=(const Single&&) = delete;


private:
	static T* _pInstance;
	static AutoRelease _autoRelease;
	static std::mutex _mutex;
};

template<typename T>
T* Single<T>::_pInstance = nullptr;

template<typename T>
Single<T>::AutoRelease Single<T>::_autoRelease;

template<typename T>
std::mutex Single<T>::_mutex;

//��̬����ʵ�ַ�ʽ
template<typename T>
class Singleton {
public:
	static T& getInstence() noexcept(std::is_nothrow_constructible<T>::value) {
		static T instance;
		return instance;
	}

	Singleton(const Singleton&) = delete;
	Singleton& operator =(const Singleton&) = delete;
	//Singleton(const Singleton&&) = delete;
	//Singleton& operetor=(const Singleton&& cls) = delete;
protected:

	Singleton() = default;
	virtual ~Singleton() noexcept = default;
	static shared_ptr<T> _test;
};

template<typename T>
shared_ptr<T> Singleton<T>::_test = nullptr;
#endif // !__SINGLE_HPP__
