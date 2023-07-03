#pragma once

#include "head.hpp"
#include "Single.hpp"

enum class infoType { moduleHandle, moduleName, moduleAddr, moduleData, moduleSize };
enum class errorType { successDone = 1, readErr, writeErr, otherErr };

#ifdef _DEBUG
#define MACRO_TRACE(format, ...) \
	printf("File: \"%s\", Function: %s, Line: %d -> ", __FILE__, __FUNCTION__, __LINE__);\
	printf(format, ##__VA_ARGS__)
#else
#define MACRO_TRACE(format, ...)
#endif // _DEBUG

namespace BaseFunc
{
	//错误判断
	void errorCheck(const char* content, bool state = false);

	//警告信息
	void warningInfo(const char* content, bool state = false);

	//读远程进程内存
	SIZE_T readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size);

	//写远程进程内存
	SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size);

	//读远程进程内存
	template<typename T>
	shared_ptr<T> readMemory(HANDLE handle, LPCVOID addr, SIZE_T count)
	{
		shared_ptr<T> pRes(new T[count]());
		readMemory(handle, addr, (LPVOID)pRes.get(), count * sizeof(T));
		return pRes;
	}

	//写远程进程内存
	template<typename T>
	SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T count)
	{
		return writeMemory(handle, addr, pBuff, count * sizeof(T));
	}

	//申请远程进程内存
	LPVOID allocMemory(SIZE_T size);

	//释放远程进程内存
	void freeMemory(LPVOID pAddr);
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

        float const * floatArray()
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
                BaseFunc::errorCheck("下标访问越界");

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

        float const * floatArray()
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
                BaseFunc::errorCheck("下标访问越界");

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