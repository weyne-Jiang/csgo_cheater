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
 * �̰߳�ȫ��˫������У���ȡ��д���������������д��Ͷ�ȡ�߳�֮��Ի�����Դ�ľ�����ʵ������������10%-20%��
 * @tparam ElemType ���Ԫ������
 */
template<typename ElemType>
class Dll_Export_Import DoubleBufferQueue
{
public:
    using SizeType = typename std::list<ElemType>::size_type;

    DoubleBufferQueue() noexcept = default;

    /*!
     * ����Ԫ�����
     * @param element Ԫ��
     * @return ��Ӻ���гߴ�
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
     * ����Ԫ����ӣ��ƶ����壩
     * @param element Ԫ��
     * @return �ɹ���Ӻ�д���гߴ�
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
     * ���Ԫ�����
     * @param elementList Ԫ���б�
     * @return Ԫ�سɹ���Ӻ�д���гߴ�
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
     * ���Ԫ����ӣ��ƶ����壩
     * @param elementList Ԫ���б�
     * @return Ԫ�سɹ���Ӻ�д���гߴ�
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
     * ��ѯ����ͷ��Ԫ��
     * @return ͷ��Ԫ�ع����optional����
     */
    [[nodiscard]]
    [[maybe_unused]]
    inline std::optional<ElemType> front()
    {
        std::lock_guard<std::mutex> lock_r(_readLock);
        if (size() == 0) {
            return std::nullopt;
        }
        //��������Ϊ�գ�����д�������ݽ���
        if (_readQueue.empty())
        {
            std::lock_guard<std::mutex> lock_w(_writeLock);
            _writeQueue.swap(_readQueue);
        }
        return std::optional(_readQueue.front());
    }

    /*!
     * ��ȡһ��Ԫ�أ����Ӷ����е���
     * @return Ԫ��
     */
    [[nodiscard]]
    [[maybe_unused]]
    inline std::optional<ElemType> pop()
    {
        std::lock_guard<std::mutex> lock_r(_readLock);
        if (size() == 0) {
            return std::nullopt;
        }
        //��������Ϊ�գ�����д�������ݽ���
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
        //atomicԭ�Ӳ�����memory_order_relaxed�ڴ�˳��
        return _queueSize.fetch_add(count, std::memory_order_relaxed);
    }

    SizeType sub(SizeType count) noexcept
    {
        return _queueSize.fetch_sub(count, std::memory_order_relaxed);
    }

private:
    std::list<ElemType> _writeQueue; //д����
    std::list<ElemType> _readQueue; //������
    std::mutex _writeLock; //д��
    std::mutex _readLock; //����
    std::atomic<SizeType> _queueSize = 0; //����Ԫ��������
};

#endif //CSGO_CHEATER_DOUBLEBUFFERQUEUE_H
