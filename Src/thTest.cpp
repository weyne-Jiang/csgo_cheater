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
    //使用硬件熵源的非确定随机数生成器（随机数引擎）
    static std::random_device rd;
    //32 位梅森缠绕器，由松本与西村设计于 1998
    static std::mt19937 mt(rd());
    //产生在一个范围上均匀分布的整数值
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
        int _id; //工作id
        ThreadPool * _pool;//所属线程池
    };

public:
    ThreadPool(const int n_threads)
            : _threads(std::vector<std::thread>(n_threads))
            , _shutdown(false)
            {

            }

    ThreadPool(const ThreadPool &) = delete; //拷贝构造函数，并且取消默认父类构造函数
    ThreadPool(ThreadPool &&) = delete; // 拷贝构造函数，允许右值引用
    ThreadPool & operator=(const ThreadPool &) = delete; // 赋值操作
    ThreadPool & operator=(ThreadPool &&) = delete; //赋值操作

    void init()
    {
        for (int i = 0; i < _threads.size(); ++i)
        {
            _threads[i] = std::thread(ThreadWorker(this, i));
        }
    }

    void shutdown() {
        _shutdown = true;
        _conditional_lock.notify_all(); //通知 唤醒所有工作线程

        for (int i = 0; i < _threads.size(); ++i)
        {
            if(_threads[i].joinable())  //判断线程是否正在等待
                _threads[i].join();  //将线程加入等待队列
        }
    }

    template<typename Func, typename ...Args>
    auto submit(Func&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        //通过完美转发and bind，构造回调函数
//        std::function<decltype(f(args...))()> func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
        //由于此时无法确定bind后返回的函数对象类型，若使用std::function<decltype(f(args...))()>无法推导具体类型，故使用auto推导
        auto func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);

        //任务打包，需要传递到另一个线程，故在堆上申请
        auto pTask = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        std::function<void()> funcWrapper = [pTask, func, args...]()
        {
            func(args...);
//            (*pTask)();
        };

        _queue.enqueue(funcWrapper);
        _conditional_lock.notify_one();
        //返回期望
        return pTask->get_future();
    }
private:
    bool _shutdown; //线程池是否关闭
    SafeQueue<std::function<void()>> _queue;//执行函数安全队列，即任务队列
    std::vector<std::thread> _threads; //工作线程队列
    std::mutex _conditional_mutex;//线程休眠锁互斥变量
    std::condition_variable _conditional_lock; //线程环境锁，让线程可以处于休眠或者唤醒状态
};

//设置线程睡眠时间
void simulate_hard_computation() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500 + uniform_random<int>(-1000, 1000)[0]));
}

// 添加两个数字的简单函数并打印结果
void multiply(const int a, const int b) {
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
}

//添加并输出结果
void multiply_output(int & out, const int a, const int b) {
    simulate_hard_computation();
    out = a * b;
    std::cout << a << " * " << b << " = " << out << std::endl;
}

// 结果返回
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

    // 使用ref传递的输出参数提交函数
    int output_ref;
    auto future1 = pool.submit(multiply_output, std::ref(output_ref), 5, 6);

    // 等待乘法输出完成
    future1.get();
    std::cout << "Last operation result is equals to " << output_ref << std::endl;

    // 使用return参数提交函数
    auto future2 = pool.submit(multiply_return, 5, 3);

    // 等待乘法输出完成
    int res = future2.get();
    std::cout << "Last operation result is equals to " << res << std::endl;

    pool.shutdown();
//    std::this_thread::sleep_for(1000ms);
    return 0;
}

