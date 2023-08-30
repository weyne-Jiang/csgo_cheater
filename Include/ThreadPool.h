//
// Created by weyne-Jiang on 2023/8/23.
//

#ifndef CSGO_CHEATER_THREADPOOL_H
#define CSGO_CHEATER_THREADPOOL_H
#include "future"
#include "vector"
#include "functional"
#include "DoubleBufferQueue.h"

//生产者打包任务，预存任务期望结果，任务执行入队
//消费者出队，执行任务，同时伴随相关策略，线程最小最大保持数量，线程超时保持退出，其他退出条件
//安全队列

#ifdef _WIN32
    #ifdef Dynamic_Build
        #define Dll_Export_Import __declspec(dllexport)
    #elif defined(Static_Build)
        #define Dll_Export_Import
    #else
        #define Dll_Export_Import
    #endif
#else
    #define Dll_Export_Import
#endif

class Dll_Export_Import ThreadPool
{
public:
    /*!
     * 线程池
     * @param minThreadCount 最小保持的线程数
     * @param maxThreadCount 最大保持的线程数
     * @param timeOut 超过最小保持线程数的线程超时时间
     */
    [[maybe_unused]]
    ThreadPool(uint16_t minThreadCount, uint16_t maxThreadCount, uint64_t timeOut);

    ~ThreadPool();

    /*!
     * 当前设备最大线程数
     * @return
     */
    [[nodiscard]]
    [[maybe_unused]]
    static uint32_t deviceMaxThread() noexcept;

    /*!
     * 设置当前线程池最小线程数
     * @param min
     */
    [[maybe_unused]]
    void setMinThread(uint16_t min) noexcept;

    /*!
     * 设置当前线程池最大线程数
     * @param max
     */
    [[maybe_unused]]
    void setMaxThread(uint16_t max) noexcept;

    /*!
     * 提交任务到线程池
     * @tparam Func 函数类型
     * @tparam Args 参数包
     * @param function 函数对象
     * @param args 参数
     * @return 任务的期望（结果）
     */
    template<typename Func, typename ...Args>
    [[maybe_unused]]
    inline auto submit(Func &&function, Args &&...args) -> std::future<decltype(function(args...))>
    {
        using ResultType = decltype(function(args...));
        //任务打包
        auto task = std::make_shared<std::packaged_task<ResultType()>>(std::bind(std::forward<Func>(function), std::forward<Args>(args)...));
        //预存任务的期望（结果）
        std::future<ResultType> taskRes = task->get_future();
        {
            std::unique_lock<std::mutex> lock(_mutex);
            //线程池已退出
            if (_exit)
                throw std::runtime_error("ThreadPool: Already exited.");
            //将任务存入任务队列
            _taskQueue.push([task](){
                (*task)();
            });
        }

        //若有空闲线程，且线程数未到最大线程，创建新线程
        if (!_enableThreadCount && _currentThreadCount < _maxThreadCount)
            createThread();
        _cond.notify_one();
        return taskRes;
    }

private:
    /*!
     * 创建线程
     */
    void createThread();

    /*!
     * 线程工作函数
     */
    void threadWorking();


private:
    uint16_t _minThreadCount = 0;
    uint16_t _maxThreadCount = 0;
    uint16_t _currentThreadCount = 0;
    uint16_t _enableThreadCount = 0;
    uint64_t _timeOut = 0;
    std::vector<std::thread> _workingThreads{};
    DoubleBufferQueue<std::function<void()>> _taskQueue{};
    bool _exit = false;
    std::mutex _mutex;
    std::condition_variable _cond;

};

#endif //CSGO_CHEATER_THREADPOOL_H
