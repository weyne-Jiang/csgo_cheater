#pragma once

#include "head.hpp"

class HelpFunc
{
public:
	HelpFunc() = delete;
	~HelpFunc() = delete;

	//错误判断
	static  void errorCheck(bool state, const char* content = nullptr);

	//警告信息
	static  void warningInfo(const char* content, bool state = false);

	//读远程进程内存
	static  SIZE_T readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size);

	//写远程进程内存
	static  SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size);

	//申请远程进程内存
	static  LPVOID allocMemory(SIZE_T size);

	//释放远程进程内存
	static  void freeMemory(LPVOID pAddr);

	//通过进程名获取进程ID
	static DWORD getProcessId(string processName);

	//通过进程ID获取进程句柄
	static HANDLE getProcessHandle(DWORD processId, DWORD access = PROCESS_ALL_ACCESS);

};
