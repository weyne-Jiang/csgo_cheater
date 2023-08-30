#ifndef __SINGLE_HPP__
#define __SINGLE_HPP__

#include <mutex>

#define SingleClass(className) \
class className : public Single<className>\
{\
friend class Single<className>;

#define Init_SingleStatic(className) \
template<>\
void* Single<className>::_pInstance = nullptr;\
template<>\
typename Single<className>::AutoRelease Single<className>::_autoRelease;\
template<>\
std::mutex Single<className>::_mutex;

//����ʽ���ӳټ��أ�����ģ��
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
				_pInstance = (void *)new T(std::forward<Args>(args)...);
		}
		return (T*)_pInstance;
	}

	T* operator->() const
	{
		return getInstence();
	}

    Single(const Single&) = delete;
    Single& operator=(const Single&) = delete;

protected:
	//�����Զ����� RAII��
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
	static void * _pInstance;
	static AutoRelease _autoRelease;
	static std::mutex _mutex;
};

//template<typename T>
//void* Single<T>::_pInstance = nullptr;
//
//template<typename T>
//typename Single<T>::AutoRelease Single<T>::_autoRelease;
//
//template<typename T>
//std::mutex Single<T>::_mutex;


#endif // !__SINGLE_HPP__
