#include "Base.hpp"

namespace BaseFunc
{
	void errorInfo(const std::string& content)
	{
		string errorInfo;
		errorInfo += "错误 ： <" + content + ", 错误码：" + std::to_string(GetLastError()) + ">";
		MACRO_TRACE("%s", errorInfo.c_str());
		//预留日志记录
#ifdef _DEBUG
		MessageBox(nullptr, (LPCSTR)errorInfo.c_str(), (LPCSTR)"错误信息", MB_OK);
#endif
		exit(-1);
	}

	void warningInfo(const std::string& content)
	{
		string warnInfo;
		warnInfo += "警告 ： <" + content + ", 错误码：" + std::to_string(GetLastError()) + ">";
		//预留日志记录
		MACRO_TRACE("%s", warnInfo.c_str());
#ifdef _DEBUG
		MessageBox(nullptr, (LPCSTR)warnInfo.c_str(), (LPCSTR)"警告信息", MB_OK);
#endif
	}

	SIZE_T readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size)
	{
		SIZE_T readSize = 0;
		if (!ReadProcessMemory(handle, addr, pBuff, size, &readSize))
			warningInfo("读取进程内存失败");
		return readSize;
	}

	SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size)
	{
		SIZE_T writeSize = 0;
		if (!WriteProcessMemory(handle, addr, pBuff, size, &writeSize))
			warningInfo("写入进程内存失败");
		return writeSize;
	}

	LPVOID allocMemory(SIZE_T size, LPVOID lpAddress)
	{
		LPVOID res = VirtualAlloc(lpAddress, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!res)
		    warningInfo("本进程内存申请失败");
		return res;
	}

    LPVOID allocMemoryEx(HANDLE hProcess, SIZE_T size, LPVOID lpAddress)
    {
        LPVOID res = VirtualAllocEx(hProcess, lpAddress, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!res)
            warningInfo("远程进程内存申请失败");
        return res;
    }

	void freeMemory(LPVOID pAddr)
	{
		if (pAddr)
		{
			//type为MEM_RELEASE时，dwsize必须为0，表示释放初始的整片区域
            if (!VirtualFree(pAddr, 0, MEM_RELEASE))
			    warningInfo("本内存释放失败");
		}
		else
			warningInfo("对空指针进行内存释放");
		pAddr = nullptr;
	}

    void freeMemory(HANDLE hProcess, LPVOID pAddr)
    {
        if (pAddr)
        {
            //type为MEM_RELEASE时，dwsize必须为0，表示释放初始的整片区域
            if (!VirtualFreeEx(hProcess, pAddr, 0, MEM_RELEASE))
                warningInfo("本内存释放失败");
        }
        else
            warningInfo("对空指针进行内存释放");
        pAddr = nullptr;
    }
}
