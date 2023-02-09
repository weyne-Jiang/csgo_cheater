#pragma once

#include "head.hpp"

namespace BaseFunc
{
	//�����ж�
	void errorCheck(bool state, const char* content = nullptr);

	//������Ϣ
	void warningInfo(const char* content, bool state = false);

	//��Զ�̽����ڴ�
	SIZE_T readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size);

	//дԶ�̽����ڴ�
	SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size);

	//��Զ�̽����ڴ�
	template<typename T>
	shared_ptr<T> readMemory(HANDLE handle, LPCVOID addr, SIZE_T count)
	{
		shared_ptr<T> pRes(new T[count]());
		readMemory(handle, addr, (LPVOID)pRes.get(), count * sizeof(T));
		return pRes;
	}

	//дԶ�̽����ڴ�
	template<typename T>
	SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T count)
	{
		return writeMemory(handle, addr, pBuff, count * sizeof(T));
	}

	//����Զ�̽����ڴ�
	LPVOID allocMemory(SIZE_T size);

	//�ͷ�Զ�̽����ڴ�
	void freeMemory(LPVOID pAddr);

	//ͨ����������ȡ����ID
	DWORD getProcessId(string processName);

	//ͨ������ID��ȡ���̾��
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

        //������ĳһ��ľ���
        float distance(Point3D& point)
        {
            return sqrtf((point.x - x) * (point.x - x) + (point.y - y) * (point.y - y) + (point.z - z) * (point.z - z));
        }

        // �����������
        static float distance(Point3D& p1, Point3D& p2) {
            return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z));
        }

        // ���ؼӷ� 
        Point3D operator + (const Point3D& r) {
            return Point3D(x + r.x, y + r.y, z + r.z);
        }
        // ���ؼ���
        Point3D operator - (const Point3D& r) {
            return Point3D(x - r.x, y - r.y, z - r.z);
        }

        //������������
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

        //������ĳһ��ľ���
        float distance(Point2D& point)
        {
            return sqrtf((point.x - x) * (point.x - x) + (point.y - y) * (point.y - y));
        }

        // �����������
        static float distance(Point2D& p1, Point2D& p2) {
            return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
        }

        // ���ؼӷ� 
        Point2D operator + (const Point2D& r) {
            return Point2D(x + r.x, y + r.y);
        }
        // ���ؼ���
        Point2D operator - (const Point2D& r) {
            return Point2D(x - r.x, y - r.y);
        }

        //������������
        friend std::ostream& operator << (std::ostream& os, const Point2D& p) {
            os << '(' << p.x << ',' << p.y << ')';
            return os;
        }
    };
}