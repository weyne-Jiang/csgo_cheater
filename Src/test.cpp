//
// Created by weyne-Jiang on 2023/8/1.
//
#undef _HAS_STD_BYTE
#include<iostream>
#include <typeinfo>
#include <queue>
#include "Timer.h"
#include "Single.hpp"
#include "Base.hpp"
#include "ThreadPool.h"
using namespace std::placeholders;
using namespace std::chrono;

class ElapsedTimer
{
public:
    ElapsedTimer(bool do_start = false)
    {
        if (do_start)
            start();
    }

    void start()
    {
        m_start_point = std::chrono::high_resolution_clock::now();
    }
    //ms
    uint64_t get_elapsed_ms()
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now-m_start_point).count();
    }
    //us
    uint64_t get_elapsed_us()
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now-m_start_point).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_point;
};

SingleClass(aaa)
public:
    int a = 0;
    double b = 2341.23;
    void pp()
    {
        printf("123213");
    }
private:
    aaa() = default;
};

Init_SingleStatic(aaa)

SingleClass(bbb)
public:
    int a = 0;
    double b = 2341.23;
    void pp()
    {
        printf("123213");
    }
private:
    bbb() = default;

};

Init_SingleStatic(bbb)

//template<typename T>
//void* Single<T>::_pInstance = nullptr;
//
//template<typename T>
//typename Single<T>::AutoRelease Single<T>::_autoRelease;
//
//template<typename T>
//std::mutex Single<T>::_mutex;

const long nGlobalArraySize = 235 * 1024 * 1024; //
static long long arrayG[nGlobalArraySize];

static const int MAX = 10e8;
static double sum = 0;

static mutex exclusive;

void concurrent_worker(int min, int max) {
    for (int i = min; i <= max; i++) {
        exclusive.lock(); // ①
        sum += sqrt(i);
        exclusive.unlock(); // ②
    }
}

void concurrent_worker1(int min, int max) {
    double tmp_sum = 0;
    for (int i = min; i <= max; i++) {
        tmp_sum += sqrt(i); // ①
    }
    exclusive.lock(); // ②
    sum += tmp_sum;
    exclusive.unlock();
}

void concurrent_task(int min, int max) {
    auto start_time = std::chrono::steady_clock::now();

    unsigned concurrent_count = thread::hardware_concurrency();
    cout << "hardware_concurrency: " << concurrent_count << endl;
    vector<thread> threads;
    min = 0;
    sum = 0;
    for (int t = 0; t < concurrent_count; t++) {
        int range = max / concurrent_count * (t + 1);
        threads.push_back(thread(concurrent_worker1, min, range)); // ③
        min = range + 1;
    }
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Concurrent task finish, " << ms << " ms consumed, Result: " << sum << endl;
}

void worker(int min, int max) { // ③
    for (int i = min; i <= max; i++) {
        sum += sqrt(i);
    }
}

void serial_task(int min, int max) { // ④
    auto start_time = std::chrono::steady_clock::now();
    sum = 0;
    worker(0, MAX);
    auto end_time = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Serail task finish, " << ms << " ms consumed, Result: " << sum << endl;
}

double concurrent_workerf(int min, int max) {
    double sum = 0;
    for (int i = min; i <= max; i++) {
        sum += sqrt(i);
    }
    return sum;
}
using std::future;
using std::packaged_task;
double concurrent_taskf(int min, int max) {
    vector<std::future<double>> results; // ①

    unsigned concurrent_count = thread::hardware_concurrency();
    min = 0;
    for (int i = 0; i < concurrent_count; i++) { // ②
        packaged_task<double(int, int)> task(concurrent_workerf); // ③
        results.push_back(task.get_future()); // ④

        int range = max / concurrent_count * (i + 1);
        thread t(std::move(task), min, range); // ⑤
        t.detach();

        min = range + 1;
    }

    cout << "threads create finish" << endl;
    double sum = 0;
    for (auto& r : results) {
        sum += r.get(); // ⑥
    }
    return sum;
}

int main11() {
    auto start_time = std::chrono::steady_clock::now();

    double r = concurrent_taskf(0, MAX);

    auto end_time = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Concurrent task finish, " << ms << " ms consumed, Result: " << r << endl;
    return 0;
}

int main12(){
//    aaa aa;
//    aaa a1;
    serial_task(1, 10e8);
    concurrent_task(1, 10e8);

    unsigned concurrent_count = thread::hardware_concurrency(); // ①
    cout << "hardware_concurrency: " << concurrent_count << endl;
    auto a1 = aaa::getInstence();
    a1->a = 666;

    auto a2 = aaa::getInstence();
    a2->a = 777;

    auto b1 = bbb::getInstence();
    b1->a = 666;

    Timer timer;
    timer.start();
    uint32_t count = 0;
    while (true)
    {
        timer.blockThread(100, TimeAccuracy::Millisecond);
        if (count++ % 2)
            timer.pause();
        else
            timer.start();

        if (count > 100)
            break;
    }
    cout << "result : "<< timer.totalTime() << endl;


//    long long* array[16];
//    for (int i = 0; i < 16; ++i) {
//        array[i] = new long long [nGlobalArraySize]();
//        cout << i << endl;
//    }
//    auto * parray = new long long [nGlobalArraySize]();
//    cout << "size of static_var : " << sizeof(*parray)*nGlobalArraySize /1024/1024 << " M\n";
//
//    TIMERSTART(testh);
//    for(long i= 0;i<nGlobalArraySize;++i)
//        parray[i] =0;
//    TIMEREND(testh);
//    DURATION_ms(testh);


    return 0;
}

void thread_computer(std::promise<int>& proObj)
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    proObj.set_value(22);
}

void foo()
{
    std::cout << "foo()" << std::endl;
    throw std::runtime_error("Error");
}

template <typename T>
class SafeQueue
{
private:
    std::queue<T> m_queue; //利用模板函数构造队列
    std::mutex m_mutex; // 访问互斥信号量
public:
    SafeQueue() {}
    SafeQueue(SafeQueue &&other) {}
    ~SafeQueue() {}
    bool empty() // 返回队列是否为空
    {
        std::unique_lock<std::mutex> lock(m_mutex); // 互斥信号变量加锁，防止m_queue被改变
        return m_queue.empty();
    }
    int size()
    {
        std::unique_lock<std::mutex> lock(m_mutex); // 互斥信号变量加锁，防止m_queue被改变
        return m_queue.size();
    }

    // 队列添加元素
    void enqueue(T &t)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.emplace(t);
    }

    // 队列取出元素
    bool dequeue(T &t)
    {
        std::unique_lock<std::mutex> lock(m_mutex); // 队列加锁
        if (m_queue.empty())
            return false;
        t = std::move(m_queue.front()); // 取出队首元素，返回队首元素值，并进行右值引用
        m_queue.pop(); // 弹出入队的第一个元素
        return true;
    }
};

SafeQueue<std::function<void()>> m_queue;

// Submit a function to be executed asynchronously by the pool
template <typename F, typename... Args>
auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
{
    // Create a function with bounded parameter ready to execute
    std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...); // 连接函数和参数定义，特殊函数类型，避免左右值错误
    // Encapsulate it into a shared pointer in order to be able to copy construct
    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
    // Warp packaged task into void function
    std::function<void()> warpper_func = [task_ptr]()
    {
        (*task_ptr)();
    };
    // 队列通用安全封包函数，并压入安全队列
    m_queue.enqueue(warpper_func);
    // 唤醒一个等待中的线程
//    m_conditional_lock.notify_one();
    // 返回先前注册的任务指针
    return task_ptr->get_future();
}

void func(int &n1, int& n2, const int&n3)
{
    std::cout << "In function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
    cout << "Address n1 : " << &n1 << endl;
    cout << "Address n2 : " << &n2 << endl;
    cout << "Address n3 : " << &n3 << endl;
    ++n1; // increments the copy of n1 stored in the function object
    ++n2; // increments the main()'s n2
//     ++n3; // compile error
}


class testfang
{
public:
    testfang()
    {
        cout << "this is testfang()\n";
    }

    testfang(int a)
    {
        cout << "this is testfang(int a)\n";
    }

    void operator()()
    {
        cout << "this operaeraraerear()\n";
    }
};

template<typename ...Args>
void testbind(Args&&... args)
{
    std::bind([&args...](){
        ([&args]() {
            cout << args << endl;
        }(), ...);// 折叠表达式
        }, args...);
    printf("args\n");
}

template<typename ...T>
void Func2(T... args)
{
    ([&args]() {
        cout << args << endl;
    }(), ...);// 折叠表达式
}

void test1()
{
    Func2(2,"32e321",4);
}

class bindCls
{
public:
    template<typename ...T>
    void print(T... t)
    {
        Func2(t...);
        auto haha = std::bind(&bindCls::Reset<T&...>, this, t...);
        haha();
    }

    template<typename Func, typename ...Args>
    auto submit(Func&& f, Args&&... args)
    {
        auto func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
        cout << typeid(func).name() << endl;
        func(std::forward<Args>(args)...);
    }

    template<typename... T>
    void Reset(T&... args)
    {
        cout << "Reset\n";
        Func2(args...);
        cout << __FUNCTION__ << endl;
        cout << __LINE__ << endl;
    }
};

void testfunc(int a)
{
    cout << "this is testfunc\n";
}

int main123()
{
    bindCls cls;
    cls.print(2, "2", "1231");
    cls.submit(&testfunc, 2);

    test1();
    testfang();
    std::thread th1(testfang(1));
    th1.join();

    int n1=1;int n2=2;int n3 =3;
    cout << "n1 size : " << sizeof(n1) << endl;
    cout << "Address n1 : " << &n1 << endl;
    cout << "Address n2 : " << &n2 << endl;
    cout << "Address n3 : " << &n3 << endl;
    std::function<void()> lala_func = std::bind(func, n1, std::ref(n2), std::cref(n3));
    std::cout << "Before function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
    lala_func();
    std::cout << "After function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
    return 0;
//    SafeQueue<int> testq;
//    decltype(testq) aaa;
//    cout << typeid(aaa).name() << endl;
//
//    try
//    {
//        std::cout << "1" << std::endl;
//        auto f = std::async(std::launch::async, foo);
//        f.get();
//        std::cout << "2" << std::endl;
//    }
//    catch (const std::exception& ex)
//    {
//        std::cerr << ex.what() << std::endl;
//    }
//    return 0;
//
//    std::cout << "Test 1 start" << std::endl;
//    auto fut1 = std::async(std::launch::async, [] { std::this_thread::sleep_for(std::chrono::milliseconds(3000)); std::cout << "work done 1!\n";
//        return 1;}); // 这一步没有阻塞，因为async的返回的future对象用于move构造了fut1，没有析构
//
//    std::cout << "Work done - implicit join on fut1 associated thread just ended\n\n";
//
//    std::cout << "Test 2 start" << std::endl;
//    std::async(std::launch::async, [] { std::this_thread::sleep_for(std::chrono::milliseconds(3000)); std::cout << "work done 2!" << std::endl; });// 这一步竟然阻塞了！因为async返回future对象是右值，将要析构，而析构会阻塞
//    std::cout << "This shold show before work done 2!?" << std::endl;
//    return 0;

    std::promise<int> proObj;
    std::future<int> futureObj = proObj.get_future();
    std::future<int> futureObj1 = proObj.get_future();
//    auto aa = std::async(&thread_computer, std::ref(proObj));

    cout << futureObj.valid() << endl;
    cout << futureObj1.valid() << endl;
    std::cout << futureObj.get() << std::endl;
//    std::cout << aa.get() << std::endl;
//    t.join();

    return 0;
}

#include <iostream>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>
#include "BlockQueue.h"
using namespace std::chrono_literals;

std::condition_variable cv;
std::mutex cv_m;
int i = 0;

void waits(int idx)
{
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk, []{return i == 1;});
    if(1)
        std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
    else
        std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
}

void signals()
{
//    std::this_thread::sleep_for(1200ms);
//    std::cerr << "Notifying...\n";
//    cv.notify_all();
//    std::this_thread::sleep_for(100ms);
    {
        std::lock_guard<std::mutex> lk(cv_m);
        i = 1;
    }
    std::cerr << "Notifying again...\n";
//    cv.notify_all();
}

//int main()
//{
//    std::thread t1(waits, 10), t2(waits, 20), t3(waits, 30);
//    std::this_thread::sleep_for(1200ms);
//    cv.notify_all();
//    cout << "111\n";
//    std::thread t4(signals);
//    std::this_thread::sleep_for(1200ms);
//    cv.notify_all();
//    t1.join(); t2.join(), t3.join(), t4.join();
//}

BlockQueue<int> block_queue;
std::mutex out_mutex;

void Producer()
{
    std::thread::id id = std::this_thread::get_id();
    while (true) {
        auto value = BaseFunc::randomUniform<int>(0, 9999);
        auto level = BaseFunc::randomUniform<int>(1, 5);
        block_queue.push(value, (Weight)level);
        {
            std::lock_guard<std::mutex> lock(out_mutex);
            BaseFunc::print("Thread id : ", id, " produces value : ", value);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Consumer()
{
    std::thread::id id = std::this_thread::get_id();
    while (true) {
        auto value = block_queue.pop();
        {
            std::lock_guard<std::mutex> lock(out_mutex);
            BaseFunc::print("Thread id : ", id, " consumes value : ", value);

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

//int main()
//{
////    Task<int> aa(Weight::level1);
////    Task<int> bb(Weight::level3);
////
////    if (aa < bb)
////        BaseFunc::print("aa < bb");
////    else
////        BaseFunc::print("aa >= bb");
////
////    std::priority_queue<Task<int>, std::vector<Task<int>>> pq;
////
////    pq.push(aa);
////    pq.push(bb);
////    BaseFunc::print("pq.top().weight ", (int)pq.top().weight);
////    pq.pop();
////    BaseFunc::print("pq.top().weight ", (int)pq.top().weight);
//
//    TIMER_START(aa)
//    for (int j = 0; j < 1024*1024; ++j)
//    {
//        block_queue.push(666, Weight::level5);
//    }
//    TIMER_END(aa)
//    TIMER_COST_ms(aa)
//
//
////    int producer_cnt = 2;
////    int consumer_cnt = 5;
////    std::vector<std::thread> producer_vec;
////    std::vector<std::thread> consumer_vec;
////    for (int i = 0; i < producer_cnt; ++i) {
////        producer_vec.emplace_back(std::thread(Producer));
////    }
////    for (int i = 0; i < consumer_cnt; ++i) {
////        consumer_vec.emplace_back(std::thread(Consumer));
////    }
////    while (true)
////    {
////        BaseFunc::print("size: ", block_queue.size());
////        std::this_thread::sleep_for(std::chrono::milliseconds(500));
////    }
////
////    for (auto &t : producer_vec) {
////        t.join();
////    }
////    for (auto &t : consumer_vec) {
////        t.join();
////    }
//    return 0;
//}



#include "DoubleBufferQueue.h"

template<typename T>
class BlockQueue1
{
public:
    BlockQueue1()
            : _capacity(NOLIMIT),
              _queue(),
              _mutex(),
              _cond()
    {
    }

    BlockQueue1(uint32_t capacity)
            : _capacity(capacity),
              _queue(),
              _mutex(),
              _cond()
    {
    }

    uint32_t size()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.size();
    }

    void push(const T &value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_capacity != NOLIMIT)
        {
            while (_queue.size() >= _capacity)
            {
                _cond.wait(lock);
            }
            assert(_queue.size() < _capacity);
        }

        _queue.push(value);
        _cond.notify_one();
    }

    void push(T &&value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_capacity != NOLIMIT)
        {
            while (_queue.size() >= _capacity)
            {
                _cond.wait(lock);
            }
            assert(_queue.size() < _capacity);
        }

        _queue.push(value);
        _cond.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_queue.size())
        {
            _cond.wait(lock);
        }

        assert(_queue.size() > 0);
        T value(std::move(_queue.front()));
        _queue.pop();
        _cond.notify_one();
        return value;
    }

private:
    uint32_t _capacity;
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
};

BlockQueue1<int> blockQueue;
DoubleBufferQueue<int> dbQueue;

void producer_blcok(const uint32_t& count)
{
    uint32_t tempC = count;
    while (--tempC)
    {
        blockQueue.push(666);
    }
}

void producer_db(const uint32_t& count)
{
    uint32_t tempC = count;
    while (--tempC)
    {
        dbQueue.push(666);
    }
}

void consumer_block(const uint32_t& count)
{
    uint32_t tempC = count;
    while (--tempC)
    {
        blockQueue.pop();
    }
}

void consumer_db(const uint32_t& count)
{
    uint32_t tempC = count;
    while (--tempC)
    {
        dbQueue.pop();
    }
}

void test11() {
    // Code inside this loop is measured repeatedly
        for (int j = 0; j < 100; ++j)
        {
            blockQueue.push(666);
        }

        for (int j = 0; j < 256; ++j)
        {
            int producer_cnt = 10;
            int consumer_cnt = 10;
            std::vector<std::thread> producer_vec;
            std::vector<std::thread> consumer_vec;
            for (int i = 0; i < producer_cnt; ++i) {
                producer_vec.emplace_back(std::thread(producer_blcok,100));
            }
            for (int i = 0; i < consumer_cnt; ++i) {
                consumer_vec.emplace_back(std::thread(consumer_block,100));
            }


            for (auto &t : producer_vec) {
                t.join();
            }
            for (auto &t : consumer_vec) {
                t.join();
            }

        }
}

void test22() {
    // Code before the loop is not measured

        for (int j = 0; j < 100; ++j)
        {
            dbQueue.push(666);
        }

        for (int j = 0; j < 256; ++j)
        {
            int producer_cnt = 10;
            int consumer_cnt = 10;
            std::vector<std::thread> producer_vec;
            std::vector<std::thread> consumer_vec;
            for (int i = 0; i < producer_cnt; ++i) {
                producer_vec.emplace_back(std::thread(producer_db,100));
            }
            for (int i = 0; i < consumer_cnt; ++i) {
                consumer_vec.emplace_back(std::thread(consumer_db,100));
            }

            for (auto &t : producer_vec) {
                t.join();
            }
            for (auto &t : consumer_vec) {
                t.join();
            }
        }
}


//[[noreturn]]
//int main()
//{
////    BENCHMARK(test22);
////    BENCHMARK(test11);
//
//
//}

#include "ThreadPool.h"
//设置线程睡眠时间
void simulate_hard_computation() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000 + BaseFunc::randomUniform<long long>(0, 1000)));
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
    ThreadPool pool(1, 3, 1000);

    // 提交乘法操作，总共30个
    for (int i = 0; i < 3; ++i) {
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

}

int main()
{
    example();
    return 0;
}
