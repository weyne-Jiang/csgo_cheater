//
// Created by weyne-Jiang on 2023/8/23.
//

#ifndef CSGO_CHEATER_THREADPOOL_H
#define CSGO_CHEATER_THREADPOOL_H
#include "future"
#include "vector"
#include "functional"
#include "DoubleBufferQueue.h"

//�����ߴ������Ԥ�������������������ִ�����
//�����߳��ӣ�ִ������ͬʱ������ز��ԣ��߳���С��󱣳��������̳߳�ʱ�����˳��������˳�����
//��ȫ����

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
     * �̳߳�
     * @param minThreadCount ��С���ֵ��߳���
     * @param maxThreadCount ��󱣳ֵ��߳���
     * @param timeOut ������С�����߳������̳߳�ʱʱ��
     */
    [[maybe_unused]]
    ThreadPool(uint16_t minThreadCount, uint16_t maxThreadCount, uint64_t timeOut);

    ~ThreadPool();

    /*!
     * ��ǰ�豸����߳���
     * @return
     */
    [[nodiscard]]
    [[maybe_unused]]
    static uint32_t deviceMaxThread() noexcept;

    /*!
     * ���õ�ǰ�̳߳���С�߳���
     * @param min
     */
    [[maybe_unused]]
    void setMinThread(uint16_t min) noexcept;

    /*!
     * ���õ�ǰ�̳߳�����߳���
     * @param max
     */
    [[maybe_unused]]
    void setMaxThread(uint16_t max) noexcept;

    /*!
     * �ύ�����̳߳�
     * @tparam Func ��������
     * @tparam Args ������
     * @param function ��������
     * @param args ����
     * @return ����������������
     */
    template<typename Func, typename ...Args>
    [[maybe_unused]]
    inline auto submit(Func &&function, Args &&...args) -> std::future<decltype(function(args...))>
    {
        using ResultType = decltype(function(args...));
        //������
        auto task = std::make_shared<std::packaged_task<ResultType()>>(std::bind(std::forward<Func>(function), std::forward<Args>(args)...));
        //Ԥ������������������
        std::future<ResultType> taskRes = task->get_future();
        {
            std::unique_lock<std::mutex> lock(_mutex);
            //�̳߳����˳�
            if (_exit)
                throw std::runtime_error("ThreadPool: Already exited.");
            //����������������
            _taskQueue.push([task](){
                (*task)();
            });
        }

        //���п����̣߳����߳���δ������̣߳��������߳�
        if (!_enableThreadCount && _currentThreadCount < _maxThreadCount)
            createThread();
        _cond.notify_one();
        return taskRes;
    }

private:
    /*!
     * �����߳�
     */
    void createThread();

    /*!
     * �̹߳�������
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
