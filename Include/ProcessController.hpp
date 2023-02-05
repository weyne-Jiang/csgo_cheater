#pragma once

#include "head.hpp"
#include "HelpFunc.hpp"

class ModuleInfo
{
public:

	ModuleInfo(HANDLE handle, DWORD processId, string moduleName);
	~ModuleInfo();
	// ģ����س�ʼ��
	void moduleInit(HANDLE handle, DWORD processId, string moduleName);
	// ��ȡģ���ַ
	BYTE* getModuleAddr() noexcept;
	// չʾָ����Ϣ
	void showInfo() = delete;

private:
	//����ģ���ڴ�ռ�
	inline void _alloc(size_t size);
	//�ͷ�ģ���ڴ�ռ�
	inline void _release();

private:
	// ģ����
	HANDLE _moduleHandle;
	string _moduleName;
	// ģ���ַ
	BYTE* _pModuleAddr;
	// ģ�����ݴ洢��ַ
	LPVOID _pModuleData;
	// ģ��ߴ�
	size_t _moduleSize;

};

class ProcessController
{
public:
	// ��ȡ���̿��ƶ���ָ��
	static ProcessController* getInstence();

	// ����ģ��
	bool loadModule(string moduleName);

	// ж��ģ��
	void unloadModule(string moduleName) = delete;

	// չʾ���н���
	void showAllProcess() = delete;

	// չʾĿ�����������ģ�� 
	void showAllModule() = delete;

	// չʾָ����Ϣ
	void showInfo() = delete;

	//ͨ����������ȡ����ID
	DWORD getProcessId(string processName);

	//ͨ������ID��ȡ���̾��
	HANDLE getProcessHandle(DWORD processId, DWORD access);

	// ��ȡ���̾��
	HANDLE getProcessHandle();

	// ��ȡ����ID
	DWORD getProcessId();

	// ��ȡģ�����ָ��
	shared_ptr<ModuleInfo> getModule(string moduleName);

	ProcessController(const ProcessController& cls) = delete;
	ProcessController(ProcessController&& cls) = delete;
	
private:
	//�����Զ����� RAII
	class AutoRelease
	{
	public:
		AutoRelease() { };
		~AutoRelease()
		{
			if (nullptr != _pInstance)
				delete _pInstance;
		}
	};

	ProcessController();
	~ProcessController() = default;

private:
	static ProcessController* _pInstance;
	static AutoRelease _autoRelease;
	
	//ԭ��ʹ��unique�������������⣬һ�����ⲿ��ȡָ��ʱ��ֻ��ͨ��move���أ�����ָ�����Ȩ��ʧ��
	//��������ⲿ���û�ȡָ�룬������ʵ�����ֻ��Ϊ��������ʹ��ʱ��ָ���ԭ����ʧ�����¶����������ʸ�Ϊshared_ptr
	map<string, shared_ptr<ModuleInfo>> _moduleMap; // ģ���ֵ��
	HANDLE _processHandle; // ���̾��
	DWORD _processId; // ����ID

};

