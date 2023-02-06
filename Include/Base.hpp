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

}