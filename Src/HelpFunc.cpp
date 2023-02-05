#include "HelpFunc.hpp"

namespace BaseFunc
{

	void errorCheck(bool state, const char* content)
	{
		if (state)
			return;
		string errorInfo;
		errorInfo += "发生错误 ： <" + string(content) + ">";
		MessageBox(nullptr, (LPCSTR)errorInfo.c_str(), (LPCSTR)"错误信息", MB_OK);
		//预留日志记录
#ifdef _DEBUG
		cout << errorInfo << endl;
#endif // DEBUG
		exit(-1);
	}

	void warningInfo(const char* content, bool state)
	{
		if (state)
			return;
		string warnInfo;
		warnInfo += "警告 ： <" + std::string(content) + ">";
		MessageBox(nullptr, (LPCSTR)warnInfo.c_str(), (LPCSTR)"警告信息", MB_OK);
		//预留日志记录
#ifdef _DEBUG
		cout << warnInfo << endl;
#endif // DEBUG

	}

	//读远程进程内存
	SIZE_T readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size)
	{
		SIZE_T readSize;
		if (!ReadProcessMemory(handle, addr, pBuff, size, &readSize))
		{
			cerr << GetLastError();
		}
		return readSize;
	}

	//写远程进程内存
	SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size)
	{
		SIZE_T writeSize;
		if (!WriteProcessMemory(handle, addr, pBuff, size, &writeSize))
		{
			cerr << GetLastError();
		}
		return writeSize;
	}

	//申请远程进程内存
	LPVOID allocMemory(SIZE_T size)
	{
		LPVOID res = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		errorCheck(res, "远程内存申请");
		return res;
	}

	//释放远程进程内存
	void freeMemory(LPVOID pAddr)
	{
		if (pAddr)
			//type为MEM_RELEASE时，dwsize必须为0，表示释放初始的整片区域
			VirtualFree(pAddr, 0, MEM_RELEASE);
		pAddr = nullptr;
	}
}
