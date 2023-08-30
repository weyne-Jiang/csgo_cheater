//
// Created by weyne-Jiang on 2023/8/5.
//
#pragma once
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include <optional>
#include <iostream>
#include <random>
#include <chrono>
using namespace std::chrono;
using namespace std::placeholders;

template<typename T>
inline std::vector<T> uniform_random(const int32_t& min, const int32_t& max, const uint32_t& count = 1)
{
    //ʹ��Ӳ����Դ�ķ�ȷ�����������������������棩
    static std::random_device rd;
    //32 λ÷ɭ�����������ɱ������������ 1998
    static std::mt19937 mt(rd());
    //������һ����Χ�Ͼ��ȷֲ�������ֵ
    std::uniform_int_distribution<T> dist(min, max);
    std::vector<T> resVec{};
    resVec.reserve(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        resVec.emplace_back(dist(mt));
    }
    return resVec;
}

template <typename T>
class SafeQueue
{
public:
    SafeQueue() = default;
    ~SafeQueue() = default;

    bool empty()
    {
        std::lock_guard<decltype(_mutex)> lock(_mutex);
        return _queue.empty();
    }

    uint16_t size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    std::optional<T> dequeue()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.empty())
            return std::nullopt;

        auto elem = std::move(_queue.front());
        _queue.pop();
        return elem;
    }

    void enqueue(const T& t)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(t);
    }
private:
    std::queue<T> _queue;
    std::mutex _mutex;
};


class ThreadPool
{
    class ThreadWorker
    {
    public:
        ThreadWorker(ThreadPool * pool, const int id)
                : _pool(pool), _id(id) {
        }

        void operator()()
        {
            while (!_pool->_shutdown)
            {
                std::unique_lock<std::mutex> lock(_pool->_conditional_mutex);
                while (_pool->_queue.empty())
                    _pool->_conditional_lock.wait(lock);

                auto res = _pool->_queue.dequeue();
                if (res != std::nullopt)
                    res.value()();
            }
        }

    private:
        int _id; //����id
        ThreadPool * _pool;//�����̳߳�
    };

public:
    ThreadPool(const int n_threads)
            : _threads(std::vector<std::thread>(n_threads))
            , _shutdown(false)
            {

            }

    ThreadPool(const ThreadPool &) = delete; //�������캯��������ȡ��Ĭ�ϸ��๹�캯��
    ThreadPool(ThreadPool &&) = delete; // �������캯����������ֵ����
    ThreadPool & operator=(const ThreadPool &) = delete; // ��ֵ����
    ThreadPool & operator=(ThreadPool &&) = delete; //��ֵ����

    void init()
    {
        for (int i = 0; i < _threads.size(); ++i)
        {
            _threads[i] = std::thread(ThreadWorker(this, i));
        }
    }

    void shutdown() {
        _shutdown = true;
        _conditional_lock.notify_all(); //֪ͨ �������й����߳�

        for (int i = 0; i < _threads.size(); ++i)
        {
            if(_threads[i].joinable())  //�ж��߳��Ƿ����ڵȴ�
                _threads[i].join();  //���̼߳���ȴ�����
        }
    }

    template<typename Func, typename ...Args>
    auto submit(Func&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        //ͨ������ת��and bind������ص�����
//        std::function<decltype(f(args...))()> func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
        //���ڴ�ʱ�޷�ȷ��bind�󷵻صĺ����������ͣ���ʹ��std::function<decltype(f(args...))()>�޷��Ƶ��������ͣ���ʹ��auto�Ƶ�
        auto func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);

        //����������Ҫ���ݵ���һ���̣߳����ڶ�������
        auto pTask = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        std::function<void()> funcWrapper = [pTask, func, args...]()
        {
            func(args...);
//            (*pTask)();
        };

        _queue.enqueue(funcWrapper);
        _conditional_lock.notify_one();
        //��������
        return pTask->get_future();
    }
private:
    bool _shutdown; //�̳߳��Ƿ�ر�
    SafeQueue<std::function<void()>> _queue;//ִ�к�����ȫ���У����������
    std::vector<std::thread> _threads; //�����̶߳���
    std::mutex _conditional_mutex;//�߳��������������
    std::condition_variable _conditional_lock; //�̻߳����������߳̿��Դ������߻��߻���״̬
};

//�����߳�˯��ʱ��
void simulate_hard_computation() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + uniform_random<int>(-1000, 1000)[0]));
}

// ����������ֵļ򵥺�������ӡ���
void multiply(const int a, const int b) {
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
}

//��Ӳ�������
void multiply_output(int & out, const int a, const int b) {
    simulate_hard_computation();
    out = a * b;
    std::cout << a << " * " << b << " = " << out << std::endl;
}

// �������
int multiply_return(const int a, const int b) {
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
    return res;
}

int main()
{
//    auto res = uniform_random<int>(-1000, 1000, 500);
//    std::cout << "size : " <<  res.size()<< std::endl;
//    for(auto &en: res)
//    {
//        std::cout << en << std::endl;
//    }

    ThreadPool pool(static_cast<int>(std::thread::hardware_concurrency()));
    pool.init();
//    std::vector<std::function<void>()> resvec;
    for (int i = 1; i < 3; ++i) {
        for (int j = 1; j < 10; ++j) {

            pool.submit(multiply, i, j);
        }
    }
//    std::this_thread::sleep_for(2000ms);
//    pool.shutdown();

    // ʹ��ref���ݵ���������ύ����
    int output_ref;
    auto future1 = pool.submit(multiply_output, std::ref(output_ref), 5, 6);

    // �ȴ��˷�������
    future1.get();
    std::cout << "Last operation result is equals to " << output_ref << std::endl;

    // ʹ��return�����ύ����
    auto future2 = pool.submit(multiply_return, 5, 3);

    // �ȴ��˷�������
    int res = future2.get();
    std::cout << "Last operation result is equals to " << res << std::endl;

    pool.shutdown();
//    std::this_thread::sleep_for(1000ms);
    return 0;
}

