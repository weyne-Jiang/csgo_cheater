//
// Created by weyne-Jiang on 2023/8/28.
//

#ifndef CSGO_CHEATER_BLOCKQUEUE_H
#define CSGO_CHEATER_BLOCKQUEUE_H

#define NOLIMIT 9999

enum class Weight
{
    level1 = 1,
    level2,
    level3,
    level4,
    level5
};

//template<typename T>
//struct Task
//{
//    Task()
//        : weight(Weight::level3)
//    {
//    }
//
//    explicit Task(const Weight& wei)
//        : weight(wei)
//    {
//    }
//
//    Task(const T& val, const Weight& wei)
//            : value(val),
//              weight(wei)
//    {
//    }
//
//    T value;
//    Weight weight;
//
//    bool operator <(const Task<T>& right) const
//    {
//        return this->weight < right.weight;
//    }
//};

template<typename T>
class BlockQueue
{
private:
    struct Task
    {
        Task() noexcept
                : weight(Weight::level3)
        {
        }

        explicit Task(const Weight& wei) noexcept
                : weight(wei)
        {
        }

        Task(const T& val, const Weight& wei) noexcept
                : value(val),
                  weight(wei)
        {
        }

        T value;
        Weight weight;

        bool operator <(const Task& right) const noexcept
        {
            return this->weight < right.weight;
        }
    };

public:
    BlockQueue() noexcept
            : _capacity(NOLIMIT),
              _queue(),
              _mutex(),
              _cond()
    {
    }

    [[maybe_unused]]
    explicit BlockQueue(uint32_t capacity) noexcept
            : _capacity(capacity),
              _queue(),
              _mutex(),
              _cond()
    {
    }

    [[nodiscard]]
    [[maybe_unused]]
    uint32_t size() noexcept
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.size();
    }

    [[maybe_unused]]
    inline void push(const T &value, const Weight& weight)
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

        Task task(value, weight);
        _queue.push(task);
        _cond.notify_one();
    }

    [[maybe_unused]]
    inline void push(T &&value, Weight && weight)
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

        Task task(value, weight);
        _queue.push(task);
        _cond.notify_one();
    }

    [[nodiscard]]
    [[maybe_unused]]
    inline T pop()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_queue.size())
        {
            _cond.wait(lock);
        }

        assert(_queue.size() > 0);
        Task task(std::move(_queue.top()));
        _queue.pop();
        _cond.notify_one();
        return task.value;
    }

private:
    uint32_t _capacity;
    std::priority_queue<Task, std::vector<Task>> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
};

#endif //CSGO_CHEATER_BLOCKQUEUE_H
