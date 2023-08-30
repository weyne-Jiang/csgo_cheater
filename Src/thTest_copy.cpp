//
// Created by weyne-Jiang on 2023/8/5.
//
//ThreadPool.h

#pragma once
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

// Thread safe implementation of a Queue using a std::queue

template <typename T>
class SafeQueue {
private:
    std::queue<T> m_queue; //利用模板函数构造队列

    std::mutex m_mutex;//访问互斥信号量

public:
    SafeQueue() { //空构造函数


    }

    SafeQueue(SafeQueue& other) {//拷贝构造函数

        //TODO:
    }

    ~SafeQueue() { //析构函数

    }


    bool empty() {  //队列是否为空

        std::unique_lock<std::mutex> lock(m_mutex); //互斥信号变量加锁，防止m_queue被改变

        return m_queue.empty();
    }

    int size() {
        std::unique_lock<std::mutex> lock(m_mutex); //互斥信号变量加锁，防止m_queue被改变

        return m_queue.size();
    }
//队列添加元素

    void enqueue(T& t) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(t);
    }
//队列取出元素

    bool dequeue(T& t) {
        std::unique_lock<std::mutex> lock(m_mutex); //队列加锁

        if (m_queue.empty()) {
            return false;
        }
        t = std::move(m_queue.front()); //取出队首元素，返回队首元素值，并进行右值引用

        m_queue.pop(); //弹出入队的第一个元素

        return true;
    }
};

class ThreadPool {
private:
    //内置线程工作类
    class ThreadWorker
    {
    private:
        int m_id; //工作id

        ThreadPool * m_pool;//所属线程池

    public:
        //构造函数

        ThreadWorker(ThreadPool * pool, const int id)
                : m_pool(pool), m_id(id) {
        }
        //重载`()`操作

        void operator()() {
            std::function<void()> func; //定义基础函数类func

            bool dequeued; //是否正在取出队列中元素

            //判断线程池是否关闭，没有关闭，循环提取

            while (!m_pool->m_shutdown) {
                {
                    //为线程环境锁加锁，互访问工作线程的休眠和唤醒

                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
                    //如果任务队列为空，阻塞当前线程

                    if (m_pool->m_queue.empty()) {
                        m_pool->m_conditional_lock.wait(lock); //等待条件变量通知，开启线程

                    }
                    //取出任务队列中的元素

                    dequeued = m_pool->m_queue.dequeue(func);
                }
                //如果成功取出，执行工作函数

                if (dequeued) {
                    func();
                }
            }
        }
    };

    bool m_shutdown; //线程池是否关闭

    SafeQueue<std::function<void()>> m_queue;//执行函数安全队列，即任务队列

    std::vector<std::thread> m_threads; //工作线程队列

    std::mutex m_conditional_mutex;//线程休眠锁互斥变量

    std::condition_variable m_conditional_lock; //线程环境锁，让线程可以处于休眠或者唤醒状态

public:
    //线程池构造函数

    ThreadPool(const int n_threads)
            : m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false) {
    }

    ThreadPool(const ThreadPool &) = delete; //拷贝构造函数，并且取消默认父类构造函数

    ThreadPool(ThreadPool &&) = delete; // 拷贝构造函数，允许右值引用

    ThreadPool & operator=(const ThreadPool &) = delete; // 赋值操作

    ThreadPool & operator=(ThreadPool &&) = delete; //赋值操作

    // Inits thread pool

    void init() {
        for (int i = 0; i < m_threads.size(); ++i) {
            m_threads[i] = std::thread(ThreadWorker(this, i));//分配工作线程
        }
    }

    // Waits until threads finish their current task and shutdowns the pool

    void shutdown() {
        m_shutdown = true;
        m_conditional_lock.notify_all(); //通知 唤醒所有工作线程

        for (int i = 0; i < m_threads.size(); ++i) {
            if(m_threads[i].joinable()) { //判断线程是否正在等待

                m_threads[i].join();  //将线程加入等待队列

            }
        }
    }

    // Submit a function to be executed asynchronously by the pool
    //线程的主要工作函数，使用了后置返回类型，自动判断函数返回值

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        // Create a function with bounded parameters ready to execute
        //

        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);//连接函数和参数定义，特殊函数类型,避免左、右值错误

        // Encapsulate it into a shared ptr in order to be able to copy construct // assign
        //封装获取任务对象，方便另外一个线程查看结果

        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // Wrap packaged task into void function
        //利用正则表达式，返回一个函数对象

        std::function<void()> wrapper_func = [task_ptr]() {
            (*task_ptr)();
        };

        // 队列通用安全封包函数，并压入安全队列

        m_queue.enqueue(wrapper_func);

        // 唤醒一个等待中的线程

        m_conditional_lock.notify_one();

        // 返回先前注册的任务指针

        return task_ptr->get_future();
    }
};

#include <iostream>
#include <random>

std::random_device rd; //真实随机数产生器

std::mt19937 mt(rd()); //生成计算随机数mt;

std::uniform_int_distribution<int> dist(-1000, 1000);//生成-1000到1000之间的离散均匀分部数

auto rnd = std::bind(dist, mt);

//设置线程睡眠时间
void simulate_hard_computation() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rnd()));
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


void example() {
    // 创建3个线程的线程池
    ThreadPool pool(3);

    // 初始化线程池
    pool.init();

    // 提交乘法操作，总共30个
    for (int i = 1; i < 3; ++i) {
        for (int j = 1; j < 10; ++j) {
            pool.submit(multiply, i, j);
        }
    }

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

    //关闭线程池
    pool.shutdown();
}

int main()
{
    example();
    return 0;
}