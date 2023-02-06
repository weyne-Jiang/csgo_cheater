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

}