#pragma once

#include "head.hpp"

namespace BaseFunc
{
	//错误判断
	void errorCheck(bool state, const char* content = nullptr);

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

	//通过进程名获取进程ID
	DWORD getProcessId(string processName);

	//通过进程ID获取进程句柄
	HANDLE getProcessHandle(DWORD processId, DWORD access = PROCESS_ALL_ACCESS);
}

namespace BaseData
{
    struct Point3D {
        float x;
        float y;
        float z;

        Point3D()
        {
            x = 0;
            y = 0;
            z = 0;
        }
    
        Point3D(float p[3]) 
        {
            x = p[0];
            y = p[1];
            z = p[2];
        }

        Point3D(float x, float y, float z) 
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        float* toFloat()
        {
            //auto p = (float[3]){x, y, z};
            float res[3] = { x,y,z };
            return res;
        }

        //计算与某一点的距离
        float distance(Point3D& point)
        {
            return sqrtf((point.x - x) * (point.x - x) + (point.y - y) * (point.y - y) + (point.z - z) * (point.z - z));
        }

        // 计算两点距离
        static float distance(Point3D& p1, Point3D& p2) {
            return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z));
        }

        // 重载加法 
        Point3D operator + (const Point3D& r) {
            return Point3D(x + r.x, y + r.y, z + r.z);
        }
        // 重载减法
        Point3D operator - (const Point3D& r) {
            return Point3D(x - r.x, y - r.y, z - r.z);
        }

        //重载输出运算符
        friend std::ostream& operator << (std::ostream& os, const Point3D& p) {
            os << '(' << p.x << ',' << p.y << ',' << p.z << ')';
            return os;
        }
    };

    struct Point2D
    {
        float x;
        float y;

        Point2D()
        {
            x = 0;
            y = 0;
        }

        Point2D(float p[2])
        {
            x = p[0];
            y = p[1];
        }

        Point2D(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        float* toFloat()
        {
            //auto p = (float[3]){x, y, z};
            float res[2] = { x,y};
            return res;
        }

        //计算与某一点的距离
        float distance(Point2D& point)
        {
            return sqrtf((point.x - x) * (point.x - x) + (point.y - y) * (point.y - y));
        }

        // 计算两点距离
        static float distance(Point2D& p1, Point2D& p2) {
            return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
        }

        // 重载加法 
        Point2D operator + (const Point2D& r) {
            return Point2D(x + r.x, y + r.y);
        }
        // 重载减法
        Point2D operator - (const Point2D& r) {
            return Point2D(x - r.x, y - r.y);
        }

        //重载输出运算符
        friend std::ostream& operator << (std::ostream& os, const Point2D& p) {
            os << '(' << p.x << ',' << p.y << ')';
            return os;
        }
    };
}