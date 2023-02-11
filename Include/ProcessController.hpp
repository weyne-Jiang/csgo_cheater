#pragma once

#include "Base.hpp"

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

	// ��ӡ��Ϣ
	void print();

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

class ProcessController : public Single<ProcessController>
{
	friend class Single<ProcessController>;
public:
	// ����ģ��
	bool loadModule(string moduleName);

	// ж��ģ��
	void unloadModule(string moduleName) = delete;

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

	//��ӡ������Ϣ
	void printProcess();

	//��ӡ������ģ����Ϣ
	void printModule();

private:

	ProcessController();
	~ProcessController() = default;

private:
	//ԭ��ʹ��unique�������������⣬һ�����ⲿ��ȡָ��ʱ��ֻ��ͨ��move���أ�����ָ�����Ȩ��ʧ��
	//��������ⲿ���û�ȡָ�룬������ʵ�����ֻ��Ϊ��������ʹ��ʱ��ָ���ԭ����ʧ�����¶����������ʸ�Ϊshared_ptr
	map<string, shared_ptr<ModuleInfo>> _moduleMap; // ģ���ֵ��
	string _processName;
	HANDLE _processHandle; // ���̾��
	DWORD _processId; // ����ID

};

