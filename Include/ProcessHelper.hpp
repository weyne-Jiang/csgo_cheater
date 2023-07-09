#pragma once
#include "Base.hpp"

//region Module
class Module
{
public:
    /*!
     * @brief ����
     */
    Module();

    /*!
     * @brief �вι���
     * @param processId ����ID
     * @param moduleName ģ����
     */
	Module(DWORD processId,const string& moduleName);

    /*!
     * @brief �вι���
     * @param processName ������
     * @param moduleName ģ����
     */
    Module(const string& processName,const string& moduleName);

    /*!
     * @brief ����
     */
	~Module();

    /*!
     * @brief ��ȡģ���ַ
     * @return ģ���ַ
     */
    [[nodiscard]]
	BYTE* moduleAddr() const noexcept;

	// չʾָ����Ϣ
	void showInfo() = delete;

	// ��ӡ��Ϣ
	void print();

private:
	HANDLE _moduleHandle; // ģ����
	string _moduleName; // ģ����
    DWORD _processId; // ��������ID
	BYTE* _pModuleAddr;	// ģ���ַ
//	LPVOID _pModuleData; // ģ�����ݴ洢��ַ
    DWORD _moduleSize;	// ģ��ߴ�
    string _modulePath; // ģ��·��
};
//endregion Module

//region Process
class Process
{
public:
    /*!
     * @brief ����
     */
    Process() = delete;

    /*!
     * @brief �вι���
     * @param processName ������
     */
    explicit Process(const string& processName);

    /*!
     * @brief ����
     */
    ~Process();

    /*!
     * @brief ����ģ��
     * @param moduleName ģ����
     * @return �Ƿ���سɹ�
     */
	bool loadModule(const string& moduleName);

	// ��ȡ���̾��
    [[nodiscard]]
	HANDLE processHandle() const noexcept;

	// ��ȡ����ID
    [[nodiscard]]
	DWORD processId() const noexcept;

    // ��ȡ������
    [[nodiscard]]
    string processName() const noexcept;

	// ��ȡģ�����ָ��
    [[nodiscard]]
	shared_ptr<Module> getModule(const string& moduleName) noexcept;

	//��ӡ������Ϣ
	void printProcess();

	//��ӡ������ģ����Ϣ
	void printModule();

    //    /*!
//     * @brief ж��ģ��
//     * @param moduleName
//     */
//	void unloadModule(string moduleName) = delete;
//
//	// չʾָ����Ϣ
//	void showInfo() = delete;

private:
	map<string, shared_ptr<Module>> _moduleMap; // ģ���ֵ��
	string _processName; // ������
	HANDLE _processHandle; // ���̾��
	DWORD _processId; // ����ID
};
//endregion

//region ProcessHelper
class ProcessHelper : public Single<ProcessHelper>
{
    friend class Single<ProcessHelper>;
public:
    /*!
     * @brief ͨ������ID+ģ��������ָ��ģ��
     * @param processId ����ID
     * @param moduleName ģ����
     * @return ģ����Ϣ
     */
    [[nodiscard]]
    static optional<MODULEENTRY32> searchModule(DWORD processId,const string& moduleName);

    /*!
     * @brief ͨ��������+ģ��������ָ��ģ��
     * @param processName ������
     * @param moduleName ģ����
     * @return ģ����Ϣ
     */
    [[nodiscard]]
    static optional<MODULEENTRY32> searchModule(const std::string& processName,const string& moduleName);

    /*!
     * @brief ͨ�����������ҽ���ID
     * @param processName ������
     * @return ����ID
     */
    [[nodiscard]]
    static optional<DWORD> searchProcessId(const std::string& processName);

    /*!
     * @brief ͨ������ID��ȡ���̾��
     * @param processId ����ID
     * @param access ����Ȩ��
     * @return ���̾����ʧ�ܷ���NULL
     */
    [[nodiscard]]
    static HANDLE getProcessHandle(DWORD processId, DWORD access = PROCESS_ALL_ACCESS);

    /*!
     * @brief ͨ����������ȡ���̾��
     * @param processName ������
     * @return ���̾��
     */
    [[nodiscard]]
    static HANDLE getProcessHandle(const string& processName);

    /*!
     * @brief ���ؽ���
     * @param processName ������
     * @return �Ƿ�ɹ�
     */
    bool loadProcess(const string& processName);

    /*!
     * @brief ��ȡ����ָ��
     * @param processName ������
     * @return ����ָ��
     */
    shared_ptr<Process> getProcess(const string& processName);

private:
    map<string, shared_ptr<Process>> _processMap; // ���̼�ֵ��
};
//endregion

