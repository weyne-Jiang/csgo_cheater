//
// Created by weyne-Jiang on 2023/8/23.
//

#ifndef CSGO_CHEATER_SAFEQUEUE_H
#define CSGO_CHEATER_SAFEQUEUE_H
#include "head.hpp"

//安全队列
//线程安全的入队出队
//利用模板可适配任意类型
//判空 检查尺寸

template <typename T>
class SafeQueue
{
public:
    SafeQueue() = default;
    SafeQueue(const SafeQueue& cls) = default;
    ~SafeQueue() = default;

    /*!
     * 判空
     * @return 是否为空
     */
    bool empty()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    int size()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.size();
    }

    void enqueue(T& elem) {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.push(elem);
    }

    T dequeue()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_queue.empty()) {
            return false;
        }

        auto elem = std::move(_queue.front()); //右值引用
        _queue.pop();
        return elem;
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
};

#endif //CSGO_CHEATER_SAFEQUEUE_H
