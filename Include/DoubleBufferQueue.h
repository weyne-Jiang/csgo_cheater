//
// Created by weyne-Jiang on 2023/8/29.
//

#ifndef CSGO_CHEATER_DOUBLEBUFFERQUEUE_H
#define CSGO_CHEATER_DOUBLEBUFFERQUEUE_H
#include <list>
#include <mutex>
#include <optional>
#include <atomic>

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

/*!
 * 线程安全的双缓冲队列，读取和写入独立操作，减少写入和读取线程之间对互斥资源的竞争（实测性能提升在10%-20%）
 * @tparam ElemType 存放元素类型
 */
template<typename ElemType>
class Dll_Export_Import DoubleBufferQueue
{
public:
    using SizeType = typename std::list<ElemType>::size_type;

    DoubleBufferQueue() noexcept = default;

    /*!
     * 单个元素入队
     * @param element 元素
     * @return 入队后队列尺寸
     */
    [[maybe_unused]]
    inline SizeType push(const ElemType &element)
    {
        std::lock_guard<std::mutex> lock(_writeLock);
        _writeQueue.emplace_back(element);
        add(1);
        return _queueSize.load();
    }

    /*!
     * 单个元素入队（移动语义）
     * @param element 元素
     * @return 成功入队后写队列尺寸
     */
    [[maybe_unused]]
    inline SizeType push(ElemType &&element)
    {
        std::lock_guard<std::mutex> lock(_writeLock);
        _writeQueue.emplace_back(element);
        add(1);
        return _queueSize.load();
    }

    /*!
     * 多个元素入队
     * @param elementList 元素列表
     * @return 元素成功入队后写队列尺寸
     */
    [[maybe_unused]]
    inline SizeType push(std::list<ElemType> &elementList) noexcept
    {
        std::lock_guard<std::mutex> lock(_writeLock);
        SizeType size = elementList.size();
        _writeQueue.splice(_writeQueue.cend(), elementList);
        add(size);
        return _queueSize;
    }

    /*!
     * 多个元素入队（移动语义）
     * @param elementList 元素列表
     * @return 元素成功入队后写队列尺寸
     */
    [[maybe_unused]]
    inline SizeType push(std::list<ElemType> &&elementList) noexcept
    {
        std::lock_guard<std::mutex> lock(_writeLock);
        SizeType size = elementList.size();
        _writeQueue.splice(_writeQueue.cend(), elementList);
        add(size);
        return _queueSize;
    }

    /*!
     * 查询队列头部元素
     * @return 头部元素构造的optional对象
     */
    [[nodiscard]]
    [[maybe_unused]]
    inline std::optional<ElemType> front()
    {
        std::lock_guard<std::mutex> lock_r(_readLock);
        if (size() == 0) {
            return std::nullopt;
        }
        //若读队列为空，将读写队列数据交换
        if (_readQueue.empty())
        {
            std::lock_guard<std::mutex> lock_w(_writeLock);
            _writeQueue.swap(_readQueue);
        }
        return std::optional(_readQueue.front());
    }

    /*!
     * 获取一个元素，并从队列中弹出
     * @return 元素
     */
    [[nodiscard]]
    [[maybe_unused]]
    inline std::optional<ElemType> pop()
    {
        std::lock_guard<std::mutex> lock_r(_readLock);
        if (size() == 0) {
            return std::nullopt;
        }
        //若读队列为空，将读写队列数据交换
        if (_readQueue.empty())
        {
            std::lock_guard<std::mutex> lock_w(_writeLock);
            _writeQueue.swap(_readQueue);
        }
        std::optional res = _readQueue.front();
        _readQueue.pop_front();
        sub(1);
        return res;
    }

    [[maybe_unused]]
    inline bool pop(std::list<ElemType> &elementList) noexcept
    {
        std::scoped_lock lock(_writeLock, _readLock);
        if (!size())
            return false;
        if (!_readQueue.empty())
            elementList.splice(elementList.cend(), _readQueue);
        if (!_writeQueue.empty())
            elementList.splice(elementList.cend(), _writeQueue);
        _queueSize = 0;
        return true;
    }

    inline void clear() {
        std::scoped_lock lock(_writeLock, _readLock);
        std::list<ElemType>().swap(_readQueue);
        std::list<ElemType>().swap(_writeQueue);
        _queueSize = 0;
    }

    [[nodiscard]]
    [[maybe_unused]]
    inline SizeType size() const noexcept
    {
        return _queueSize.load();
    }

    [[nodiscard]]
    [[maybe_unused]]
    inline bool empty() const noexcept
    {
        return !size();
    }

    [[nodiscard]]
    [[maybe_unused]]
    std::list<ElemType> operator()()
    {
        std::list<ElemType> list;
        this->pop(list);
        return list;
    }

private:
    SizeType add(SizeType count) noexcept
    {
        //atomic原子操作，memory_order_relaxed内存顺序
        return _queueSize.fetch_add(count, std::memory_order_relaxed);
    }

    SizeType sub(SizeType count) noexcept
    {
        return _queueSize.fetch_sub(count, std::memory_order_relaxed);
    }

private:
    std::list<ElemType> _writeQueue; //写队列
    std::list<ElemType> _readQueue; //读队列
    std::mutex _writeLock; //写锁
    std::mutex _readLock; //读锁
    std::atomic<SizeType> _queueSize = 0; //队列元素总数量
};

#endif //CSGO_CHEATER_DOUBLEBUFFERQUEUE_H
