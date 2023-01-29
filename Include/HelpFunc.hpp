#pragma once

#include "head.hpp"

class HelpFunc
{
public:
	HelpFunc() = delete;
	~HelpFunc() = delete;

	//�����ж�
	static  void errorCheck(bool state, const char* content = nullptr);

	//������Ϣ
	static  void warningInfo(const char* content, bool state = false);

	//��Զ�̽����ڴ�
	static  SIZE_T readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size);

	//дԶ�̽����ڴ�
	static  SIZE_T writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size);

	//����Զ�̽����ڴ�
	static  LPVOID allocMemory(SIZE_T size);

	//�ͷ�Զ�̽����ڴ�
	static  void freeMemory(LPVOID pAddr);

	//ͨ����������ȡ����ID
	static DWORD getProcessId(string processName);

	//ͨ������ID��ȡ���̾��
	static HANDLE getProcessHandle(DWORD processId, DWORD access = PROCESS_ALL_ACCESS);

};
