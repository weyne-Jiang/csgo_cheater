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
        // �������float����Ļ���ת��
        Point3D(float p[3]) {
            x = p[0];
            y = p[1];
            z = p[2];
        }
        float* GetPoint3DFloat{
            float haha[3] = {x, y, z};
            return ()float[3] {x, y, z};
        }
        // ����
        const float Distance(Point3D& p1, Point3D& p2) {
            return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z));
        }
        // ���ؼӷ� 
        const Point3D operator + (const Point3D& r) {
            return Point3D(x + r.x, y + r.y, z + r.z);
        }
        // ���ؼ���
        const Point3D operator - (const Point3D& r) {
            return Point3D(x - r.x, y - r.y, z - r.z);
        }
        // ���س˷� 
        const Point3D operator * (const Point3D& r) {
            return Point3D(x * r.x, y * r.y, z * r.z);
        }
        // ���س���
        const Point3D operator / (const Point3D& r) {
            return Point3D(x / r.x, y / r.y, z / r.z);
        }
        //������������
        friend std::ostream& operator << (std::ostream& os, const Point3D& p) {
            os << '(' << p.x << ',' << p.y << ',' << p.z << ')';
            return os;
        }
    };
}