#pragma once
#include "head.hpp"
#include "Single.hpp"

#ifdef _DEBUG
/*!
 * @brief 错误追踪
 */
#define MACRO_TRACE(format, ...) \
	printf("File: \"%s\", Function: %s, Line: %d -> ", __FILE__, __FUNCTION__, __LINE__);\
	printf(format, ##__VA_ARGS__)
#else
#define MACRO_TRACE(format, ...)
#endif // _DEBUG

namespace BaseFunc
{
    /*!
     * @brief 错误判断
     * @param content 错误内容
     */
	void errorInfo(const std::string& content);

    /*!
     * @brief 警告判断
     * @param content 警告内容
     */
	void warningInfo(const std::string& content);

    /*!
     * @brief 读远程进程内存
     * @param handle 进程句柄
     * @param addr 进程内起始地址
     * @param pBuff 读取内存缓存
     * @param size 预定读取尺寸
     * @return 实际读取尺寸
     */
	uint64_t readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size);

    /*!
     * @brief 写远程进程内存
     * @param handle 进程句柄
     * @param addr 进程内起始地址
     * @param pBuff 写入内存缓存
     * @param size 预定写入尺寸
     * @return 实际写入尺寸
     */
	uint64_t writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size);

	/*!
	 * @brief 读取进程内存
	 * @tparam T 单位数据-类型
	 * @param handle 进程句柄
	 * @param addr 进程内存起始地址
	 * @param count 读取单位数量
	 * @return 数据指针
	 */
	template<typename T>
	[[nodiscard]]
	shared_ptr<T> readMemory(HANDLE handle, LPCVOID addr, SIZE_T count)
	{
        auto pRes = new T[count]();
//		shared_ptr<T> pRes(new T[count]());
		readMemory(handle, addr, (LPVOID)pRes, count * sizeof(T));
		return std::make_shared<T>(pRes);
	}

	/*!
	 * @brief 写入进程内存
	 * @tparam T 单位数据-类型
	 * @param handle 进程句柄
	 * @param addr 进程内存起始地址
	 * @param pBuff 数据缓存
	 * @param count 读取单位数量
	 * @return 实际读取字节数
	 */
	template<typename T>
	uint64_t writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T count)
	{
		return writeMemory(handle, addr, pBuff, count * sizeof(T));
	}

    /*!
 * @brief 读远程进程内存
 * @param handle 进程句柄
 * @param addr 进程内起始地址
 * @param pBuff 读取内存缓存
 * @param size 预定读取尺寸
 * @return 实际读取尺寸
 */
    uint64_t readMemory(HANDLE handle, long addr, LPVOID pBuff, SIZE_T size);

    /*!
     * @brief 写远程进程内存
     * @param handle 进程句柄
     * @param addr 进程内起始地址
     * @param pBuff 写入内存缓存
     * @param size 预定写入尺寸
     * @return 实际写入尺寸
     */
    uint64_t writeMemory(HANDLE handle, long addr, LPVOID pBuff, SIZE_T size);

    /*!
     * @brief 读取进程内存
     * @tparam T 单位数据-类型
     * @param handle 进程句柄
     * @param addr 进程内存起始地址
     * @param count 读取单位数量
     * @return 数据指针
     */
    template<typename T>
    [[nodiscard]]
    shared_ptr<T> readMemory(HANDLE handle, long addr, SIZE_T count)
    {
        auto pRes = new T[count]();
//		shared_ptr<T> pRes(new T[count]());
        readMemory(handle, addr, (LPVOID)pRes, count * sizeof(T));
        return std::make_shared<T>(pRes);
    }

    /*!
     * @brief 写入进程内存
     * @tparam T 单位数据-类型
     * @param handle 进程句柄
     * @param addr 进程内存起始地址
     * @param pBuff 数据缓存
     * @param count 读取单位数量
     * @return 实际读取字节数
     */
    template<typename T>
    uint64_t writeMemory(HANDLE handle, long addr, LPVOID pBuff, SIZE_T count)
    {
        return writeMemory(handle, addr, pBuff, count * sizeof(T));
    }


    /*!
     * @brief 申请本进程内存
     * @param size 申请字节数
     * @param lpAddress 进程内虚拟内存地址
     * @return 页面分配区域的基址
     */
    [[nodiscard]]
	LPVOID allocMemory(SIZE_T size, LPVOID lpAddress = nullptr);

    /*!
     * @brief 申请远程进程内存
     * @param hProcess 进程句柄
     * @param size 申请字节数
     * @param lpAddress 远程进程虚拟内存地址
     * @return 页面分配区域的基址
     */
    [[nodiscard]]
    LPVOID allocMemoryEx(HANDLE hProcess, SIZE_T size, LPVOID lpAddress = nullptr);

    /*!
     * @brief 释放本进程内存
     * @param pAddr 指向要释放的页面区域的基址的指针
     */
    void freeMemory(LPVOID pAddr);

    /*!
     * @brief 释放远程进程内存
     * @param hProcess 进程句柄
     * @param pAddr 指向要释放的页面区域的基址的指针
     */
    void freeMemory(HANDLE hProcess, LPVOID pAddr);

    /*!
     * @brief 打印函数
     * @tparam Types 不用管
     * @param args 不用管
     */
    template <typename ... Types>
    void print (const Types&... args)
    {
#ifdef _DEBUG
        std::initializer_list <int> { ([&args] {std::cout << args;}(), 0)...};
        std::cout << std::endl;
#endif
    }
}

namespace BaseData
{
    class Point3D {
    public:
        Point3D()
        {
            _pPoint = new float [3]();
            _pPoint[0] = 0;
            _pPoint[1] = 0;
            _pPoint[2] = 0;
        }
    
        explicit Point3D(const float p[3])
        {
            _pPoint = new float [3]();
            _pPoint[0] = p[0];
            _pPoint[1] = p[1];
            _pPoint[2] = p[2];
        }

        Point3D(const float& x, const float& y, const float& z)
        {
            _pPoint = new float [3]();
            _pPoint[0] = x;
            _pPoint[1] = y;
            _pPoint[2] = z;
        }

        ~Point3D()
        {
            delete [] _pPoint;
        }

        [[nodiscard]]
        float& x() const
        {
            return _pPoint[0];
        }

        [[nodiscard]]
        float& y() const
        {
            return _pPoint[1];
        }

        [[nodiscard]]
        float& z() const
        {
            return _pPoint[2];
        }

        [[nodiscard]]
        float * floatPoint() const
        {
            return _pPoint;
        }

        //计算与某一点的距离
        float distance(Point3D& point) const
        {
            return sqrtf((point.x() - this->x()) * (point.x() - this->x()) + (point.y() - this->y()) * (point.y() - this->y()) + (point.z() - this->z()) * (point.z() - this->z()));
        }

        // 计算两点距离
        static float distance(Point3D& p1, Point3D& p2) {
            return sqrtf((p1.x()-p2.x())*(p1.x()-p2.x()) + (p1.y()-p2.y())*(p1.y()-p2.y()) + (p1.z()-p2.z())*(p1.z()-p2.z()));
        }

        // 重载加法 
        Point3D operator + (const Point3D& cls) const
        {
            return {this->x() + cls.x(), this->y() + cls.y(), this->z() + cls.z()};
        }

        // 重载减法
        Point3D operator - (const Point3D& cls) const
        {
            return {this->x() - cls.x(), this->y() - cls.y(), this->z() - cls.z()};
        }

        // 重载下标
        float & operator [] (const int& index) {
            if (index < 0 || index > 2)
                BaseFunc::errorInfo("下标访问越界");

            switch (index)
            {
            case 0:
                return _pPoint[0];
            case 1:
                return _pPoint[1];
            case 2:
                return _pPoint[2];
            default:
                break;
            }
        }

        //重载输出运算符
        friend std::ostream& operator << (std::ostream& os, const Point3D& p) {
            os << '(' << p.x() << ',' << p.y() << ',' << p.z() << ')';
            return os;
        }

    private:
        float *_pPoint;
    };

    class Point2D
    {
    public:
        Point2D()
        {
            _pPoint = new float [2]();
            _pPoint[0] = 0;
            _pPoint[1] = 0;
        }

        explicit Point2D(const float p[2])
        {
            _pPoint = new float [2]();
            _pPoint[0] = p[0];
            _pPoint[1] = p[1];
        }

        Point2D(const float& x, const float& y)
        {
            _pPoint = new float [2]();
            _pPoint[0] = x;
            _pPoint[1] = y;
        }

        ~Point2D()
        {
            delete [] _pPoint;
        }

        [[nodiscard]]
        float * floatPoint() const
        {
            return _pPoint;
        }

        [[nodiscard]]
        float& x() const
        {
            return _pPoint[0];
        }

        [[nodiscard]]
        float& y() const
        {
            return _pPoint[1];
        }

        //计算与某一点的距离
        float distance(Point2D& point) const
        {
            return sqrtf((point.x() - this->x()) * (point.x() - this->x()) + (point.y() - this->y()) * (point.y() - this->y()));
        }

        // 计算两点距离
        static float distance(Point2D& p1, Point2D& p2)
        {
            return sqrtf((p1.x()-p2.x())*(p1.x()-p2.x()) + (p1.y()-p2.y())*(p1.y()-p2.y()));
        }

        // 重载加法
        Point2D operator + (const Point2D& cls) const
        {
            return {this->x() + cls.x(), this->y() + cls.y()};
        }

        // 重载减法
        Point2D operator - (const Point2D& cls) const
        {
            return {this->x() - cls.x(), this->y() - cls.y()};
        }

        // 重载下标
        float & operator [] (const int& index) {
            if (index < 0 || index > 1)
                BaseFunc::errorInfo("下标访问越界");

            switch (index)
            {
                case 0:
                    return _pPoint[0];
                case 1:
                    return _pPoint[1];
                default:
                    break;
            }
        }

        //重载输出运算符
        friend std::ostream& operator << (std::ostream& os, const Point2D& p) {
            os << '(' << p.x() << ',' << p.y() << ')';
            return os;
        }

    private:
        float *_pPoint;
    };
}