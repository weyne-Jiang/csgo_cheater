//
// Created by weyne-Jiang on 2023/8/23.
//

#ifndef CSGO_CHEATER_SAFEQUEUE_H
#define CSGO_CHEATER_SAFEQUEUE_H
#include "head.hpp"

//��ȫ����
//�̰߳�ȫ����ӳ���
//����ģ���������������
//�п� ���ߴ�

template <typename T>
class SafeQueue
{
public:
    SafeQueue() = default;
    SafeQueue(const SafeQueue& cls) = default;
    ~SafeQueue() = default;

    /*!
     * �п�
     * @return �Ƿ�Ϊ��
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

        auto elem = std::move(_queue.front()); //��ֵ����
        _queue.pop();
        return elem;
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
};

#endif //CSGO_CHEATER_SAFEQUEUE_H
