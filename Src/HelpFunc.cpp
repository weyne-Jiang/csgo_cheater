#include "HelpFunc.hpp"

 void HelpFunc::errorCheck(bool state, const char* content)
{
	if (state)
		return;
	string errorInfo = "�������� �� <";
	errorInfo += content;
	errorInfo += ">";
	MessageBox(nullptr, (LPCWSTR)errorInfo.c_str(), (LPCWSTR)"������Ϣ", MB_OK);
	//Ԥ����־��¼
#ifdef _DEBUG
	cout << errorInfo << endl;
#endif // DEBUG
	exit(-1);
}

 void HelpFunc::warningInfo(const char* content, bool state)
 {
	 if (state)
		 return;
	 string warnInfo = "���� �� <";
	 warnInfo += content;
	 warnInfo += ">";
	 MessageBox(nullptr, (LPCWSTR)warnInfo.c_str(), (LPCWSTR)"������Ϣ", MB_OK);
	 //Ԥ����־��¼
#ifdef _DEBUG
	 cout << warnInfo << endl;
#endif // DEBUG

 }

//��Զ�̽����ڴ�
 SIZE_T HelpFunc::readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size)
{
	SIZE_T readSize;
	ReadProcessMemory(handle, addr, pBuff, size, &readSize);
	return readSize;
}

//дԶ�̽����ڴ�
 SIZE_T HelpFunc::writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size)
{
	SIZE_T writeSize;
	WriteProcessMemory(handle, addr, pBuff, size, &writeSize);
	return writeSize;
}

//����Զ�̽����ڴ�
 LPVOID HelpFunc::allocMemory(SIZE_T size)
{
	LPVOID res = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	HelpFunc::errorCheck(res, "Զ���ڴ�����");
	return res;
}

//�ͷ�Զ�̽����ڴ�
 void HelpFunc::freeMemory(LPVOID pAddr)
{
	if (pAddr)
		//typeΪMEM_RELEASEʱ��dwsize����Ϊ0����ʾ�ͷų�ʼ����Ƭ����
		VirtualFree(pAddr, 0, MEM_RELEASE);
	pAddr = nullptr;
}

//ͨ����������ȡ����ID
DWORD HelpFunc::getProcessId(string processName)
{
	//��������
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	errorCheck(snap != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshotʧ��");

	PROCESSENTRY32 processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	processInfo.dwSize = sizeof(processInfo);

	//��������
	bool state = Process32First(snap, &processInfo);
	while (state)
	{
		if (processName == string((char *)processInfo.szExeFile))
		{
#ifdef _DEBUG
			printf("�������� : %s \n", processInfo.szExeFile);
			printf("����ID : %d \n", processInfo.th32ProcessID);
			printf("\n");
#endif // DEBUG

			CloseHandle(snap);
			return processInfo.th32ProcessID;
		}
		state = Process32Next(snap, &processInfo);
	}
	CloseHandle(snap);
	string wainingstr = "δ��Ѱ�����̣�" + processName;
	warningInfo(wainingstr.c_str());
	return 0;
}

//ͨ������ID��ȡ���̾��
HANDLE HelpFunc::getProcessHandle(DWORD processId, DWORD access)
{
	HANDLE processHandle = OpenProcess(access, FALSE, processId);
	errorCheck(processHandle, "�򿪽��̾��ʧ��");

#ifdef _DEBUG
	printf("����ID : %d \n", processId);
	printf("���̾�� : 0x%08x \n", processHandle);
	printf("\n");
#endif // DEBUG
	return processHandle;
}

