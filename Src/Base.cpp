#include "Base.hpp"

namespace BaseFunc
{
	void errorInfo(const std::string& content)
	{
		string errorInfo;
		errorInfo += "���� �� <" + content + ", �����룺" + std::to_string(GetLastError()) + ">";
		MACRO_TRACE("%s", errorInfo.c_str());
		//Ԥ����־��¼
#ifdef _DEBUG
		MessageBox(nullptr, (LPCSTR)errorInfo.c_str(), (LPCSTR)"������Ϣ", MB_OK);
#endif
		exit(-1);
	}

	void warningInfo(const std::string& content)
	{
		string warnInfo;
		warnInfo += "���� �� <" + content + ", �����룺" + std::to_string(GetLastError()) + ">";
		//Ԥ����־��¼
		MACRO_TRACE("%s", warnInfo.c_str());
#ifdef _DEBUG
		MessageBox(nullptr, (LPCSTR)warnInfo.c_str(), (LPCSTR)"������Ϣ", MB_OK);
#endif
	}

	SIZE_T readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size)
	{
		SIZE_T readSize = 0;
		if (!ReadProcessMemory(handle, addr, pBuff, size, &readSize))
			warningInfo("��ȡ�����ڴ�ʧ��");
		return readSize;
	}

	SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size)
	{
		SIZE_T writeSize = 0;
		if (!WriteProcessMemory(handle, addr, pBuff, size, &writeSize))
			warningInfo("д������ڴ�ʧ��");
		return writeSize;
	}

	LPVOID allocMemory(SIZE_T size, LPVOID lpAddress)
	{
		LPVOID res = VirtualAlloc(lpAddress, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!res)
		    warningInfo("�������ڴ�����ʧ��");
		return res;
	}

    LPVOID allocMemoryEx(HANDLE hProcess, SIZE_T size, LPVOID lpAddress)
    {
        LPVOID res = VirtualAllocEx(hProcess, lpAddress, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!res)
            warningInfo("Զ�̽����ڴ�����ʧ��");
        return res;
    }

	void freeMemory(LPVOID pAddr)
	{
		if (pAddr)
		{
			//typeΪMEM_RELEASEʱ��dwsize����Ϊ0����ʾ�ͷų�ʼ����Ƭ����
            if (!VirtualFree(pAddr, 0, MEM_RELEASE))
			    warningInfo("���ڴ��ͷ�ʧ��");
		}
		else
			warningInfo("�Կ�ָ������ڴ��ͷ�");
		pAddr = nullptr;
	}

    void freeMemory(HANDLE hProcess, LPVOID pAddr)
    {
        if (pAddr)
        {
            //typeΪMEM_RELEASEʱ��dwsize����Ϊ0����ʾ�ͷų�ʼ����Ƭ����
            if (!VirtualFreeEx(hProcess, pAddr, 0, MEM_RELEASE))
                warningInfo("���ڴ��ͷ�ʧ��");
        }
        else
            warningInfo("�Կ�ָ������ڴ��ͷ�");
        pAddr = nullptr;
    }
}
