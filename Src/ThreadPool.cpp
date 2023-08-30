//
// Created by weyne-Jiang on 2023/8/29.
//
#include "ThreadPool.h"

[[maybe_unused]]
ThreadPool::ThreadPool(uint16_t minThreadCount, uint16_t maxThreadCount, uint64_t timeOut)
{
    if (!maxThreadCount)
        throw std::invalid_argument("ThreadPool : Max Thread Count must be larger than zero.");
    _minThreadCount = minThreadCount;
    _maxThreadCount = maxThreadCount;
    _timeOut = timeOut;
    for (int i = 0; i < minThreadCount; ++i)
    {
        createThread();
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _exit = true;
    }
    _cond.notify_all();
    for (auto &th: _workingThreads)
    {
        th.join();
    }
}

[[nodiscard]]
[[maybe_unused]]
uint32_t ThreadPool::deviceMaxThread() noexcept
{
    return std::thread::hardware_concurrency();
}

[[maybe_unused]]
void ThreadPool::setMinThread(uint16_t min) noexcept
{
    _minThreadCount = min;
    _cond.notify_all();
}

[[maybe_unused]]
void ThreadPool::setMaxThread(uint16_t max) noexcept
{
    _maxThreadCount = max;
    _cond.notify_all();
}

void ThreadPool::createThread()
{
    ++_currentThreadCount;
    _workingThreads.emplace_back([this](){
        threadWorking();
    });
}

void ThreadPool::threadWorking()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(this->_mutex);
            while (true)
            {
                //�߳������ȴ����񣬿����߳�++
                ++_enableThreadCount;
                if (_currentThreadCount > _minThreadCount)
                    //��ǰ�߳���������С�߳�������ʱ�ȴ���������֪ͨ����ʱ�Ҷ���Ϊ�գ�����false��
                    this->_cond.wait_for(lock, std::chrono::milliseconds(_timeOut), [this](){
                        return this->_exit || !this->_taskQueue.empty();
                    });
                else
                    //��ǰ�߳���������С�߳���
                    this->_cond.wait(lock, [this](){
                        return this->_exit || !this->_taskQueue.empty();
                    });
                //�̻߳��ѣ�ִ�����񣬿����߳�--
                --_enableThreadCount;
                //�߳��˳��ж�
                if (_taskQueue.empty() && (_exit || _currentThreadCount > _minThreadCount))
                    return;
                else if (!_taskQueue.empty())
                    //������в�Ϊ�գ�����ѭ��ȡ����
                    break;
            }
        }
        task = _taskQueue.pop().value();
        if (nullptr != task)
            task();
    }
}