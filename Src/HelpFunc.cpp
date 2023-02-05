#include "HelpFunc.hpp"

namespace BaseFunc
{

	void errorCheck(bool state, const char* content)
	{
		if (state)
			return;
		string errorInfo;
		errorInfo += "�������� �� <" + string(content) + ">";
		MessageBox(nullptr, (LPCSTR)errorInfo.c_str(), (LPCSTR)"������Ϣ", MB_OK);
		//Ԥ����־��¼
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
		warnInfo += "���� �� <" + std::string(content) + ">";
		MessageBox(nullptr, (LPCSTR)warnInfo.c_str(), (LPCSTR)"������Ϣ", MB_OK);
		//Ԥ����־��¼
#ifdef _DEBUG
		cout << warnInfo << endl;
#endif // DEBUG

	}

	//��Զ�̽����ڴ�
	SIZE_T readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size)
	{
		SIZE_T readSize;
		if (!ReadProcessMemory(handle, addr, pBuff, size, &readSize))
		{
			cerr << GetLastError();
		}
		return readSize;
	}

	//дԶ�̽����ڴ�
	SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size)
	{
		SIZE_T writeSize;
		if (!WriteProcessMemory(handle, addr, pBuff, size, &writeSize))
		{
			cerr << GetLastError();
		}
		return writeSize;
	}

	//����Զ�̽����ڴ�
	LPVOID allocMemory(SIZE_T size)
	{
		LPVOID res = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		errorCheck(res, "Զ���ڴ�����");
		return res;
	}

	//�ͷ�Զ�̽����ڴ�
	void freeMemory(LPVOID pAddr)
	{
		if (pAddr)
			//typeΪMEM_RELEASEʱ��dwsize����Ϊ0����ʾ�ͷų�ʼ����Ƭ����
			VirtualFree(pAddr, 0, MEM_RELEASE);
		pAddr = nullptr;
	}
}
