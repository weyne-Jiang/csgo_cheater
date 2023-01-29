#include "HelpFunc.hpp"

 void HelpFunc::errorCheck(bool state, const char* content)
{
	if (state)
		return;
	string errorInfo = "发生错误 ： <";
	errorInfo += content;
	errorInfo += ">";
	MessageBox(nullptr, (LPCWSTR)errorInfo.c_str(), (LPCWSTR)"错误信息", MB_OK);
	//预留日志记录
#ifdef _DEBUG
	cout << errorInfo << endl;
#endif // DEBUG
	exit(-1);
}

 void HelpFunc::warningInfo(const char* content, bool state)
 {
	 if (state)
		 return;
	 string warnInfo = "警告 ： <";
	 warnInfo += content;
	 warnInfo += ">";
	 MessageBox(nullptr, (LPCWSTR)warnInfo.c_str(), (LPCWSTR)"警告信息", MB_OK);
	 //预留日志记录
#ifdef _DEBUG
	 cout << warnInfo << endl;
#endif // DEBUG

 }

//读远程进程内存
 SIZE_T HelpFunc::readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size)
{
	SIZE_T readSize;
	ReadProcessMemory(handle, addr, pBuff, size, &readSize);
	return readSize;
}

//写远程进程内存
 SIZE_T HelpFunc::writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size)
{
	SIZE_T writeSize;
	WriteProcessMemory(handle, addr, pBuff, size, &writeSize);
	return writeSize;
}

//申请远程进程内存
 LPVOID HelpFunc::allocMemory(SIZE_T size)
{
	LPVOID res = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	HelpFunc::errorCheck(res, "远程内存申请");
	return res;
}

//释放远程进程内存
 void HelpFunc::freeMemory(LPVOID pAddr)
{
	if (pAddr)
		//type为MEM_RELEASE时，dwsize必须为0，表示释放初始的整片区域
		VirtualFree(pAddr, 0, MEM_RELEASE);
	pAddr = nullptr;
}

//通过进程名获取进程ID
DWORD HelpFunc::getProcessId(string processName)
{
	//创建快照
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	errorCheck(snap != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshot失败");

	PROCESSENTRY32 processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	processInfo.dwSize = sizeof(processInfo);

	//遍历进程
	bool state = Process32First(snap, &processInfo);
	while (state)
	{
		if (processName == string((char *)processInfo.szExeFile))
		{
#ifdef _DEBUG
			printf("进程名称 : %s \n", processInfo.szExeFile);
			printf("进程ID : %d \n", processInfo.th32ProcessID);
			printf("\n");
#endif // DEBUG

			CloseHandle(snap);
			return processInfo.th32ProcessID;
		}
		state = Process32Next(snap, &processInfo);
	}
	CloseHandle(snap);
	string wainingstr = "未搜寻到进程：" + processName;
	warningInfo(wainingstr.c_str());
	return 0;
}

//通过进程ID获取进程句柄
HANDLE HelpFunc::getProcessHandle(DWORD processId, DWORD access)
{
	HANDLE processHandle = OpenProcess(access, FALSE, processId);
	errorCheck(processHandle, "打开进程句柄失败");

#ifdef _DEBUG
	printf("进程ID : %d \n", processId);
	printf("进程句柄 : 0x%08x \n", processHandle);
	printf("\n");
#endif // DEBUG
	return processHandle;
}

