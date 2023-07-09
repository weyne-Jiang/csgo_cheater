#pragma once
#include "Base.hpp"

//region Module
class Module
{
public:
    /*!
     * @brief 构造
     */
    Module();

    /*!
     * @brief 有参构造
     * @param processId 进程ID
     * @param moduleName 模块名
     */
	Module(DWORD processId,const string& moduleName);

    /*!
     * @brief 有参构造
     * @param processName 进程名
     * @param moduleName 模块名
     */
    Module(const string& processName,const string& moduleName);

    /*!
     * @brief 析构
     */
	~Module();

    /*!
     * @brief 获取模块地址
     * @return 模块地址
     */
    [[nodiscard]]
	BYTE* moduleAddr() const noexcept;

	// 展示指定信息
	void showInfo() = delete;

	// 打印信息
	void print();

private:
	HANDLE _moduleHandle; // 模块句柄
	string _moduleName; // 模块名
    DWORD _processId; // 所属进程ID
	BYTE* _pModuleAddr;	// 模块地址
//	LPVOID _pModuleData; // 模块数据存储地址
    DWORD _moduleSize;	// 模块尺寸
    string _modulePath; // 模块路径
};
//endregion Module

//region Process
class Process
{
public:
    /*!
     * @brief 构造
     */
    Process() = delete;

    /*!
     * @brief 有参构造
     * @param processName 进程名
     */
    explicit Process(const string& processName);

    /*!
     * @brief 析构
     */
    ~Process();

    /*!
     * @brief 加载模块
     * @param moduleName 模块名
     * @return 是否加载成功
     */
	bool loadModule(const string& moduleName);

	// 获取进程句柄
    [[nodiscard]]
	HANDLE processHandle() const noexcept;

	// 获取进程ID
    [[nodiscard]]
	DWORD processId() const noexcept;

    // 获取进程名
    [[nodiscard]]
    string processName() const noexcept;

	// 获取模块对象指针
    [[nodiscard]]
	shared_ptr<Module> getModule(const string& moduleName) noexcept;

	//打印进程信息
	void printProcess();

	//打印进程下模块信息
	void printModule();

    //    /*!
//     * @brief 卸载模块
//     * @param moduleName
//     */
//	void unloadModule(string moduleName) = delete;
//
//	// 展示指定信息
//	void showInfo() = delete;

private:
	map<string, shared_ptr<Module>> _moduleMap; // 模块键值表
	string _processName; // 进程名
	HANDLE _processHandle; // 进程句柄
	DWORD _processId; // 进程ID
};
//endregion

//region ProcessHelper
class ProcessHelper : public Single<ProcessHelper>
{
    friend class Single<ProcessHelper>;
public:
    /*!
     * @brief 通过进程ID+模块名查找指定模块
     * @param processId 进程ID
     * @param moduleName 模块名
     * @return 模块信息
     */
    [[nodiscard]]
    static optional<MODULEENTRY32> searchModule(DWORD processId,const string& moduleName);

    /*!
     * @brief 通过进程名+模块名查找指定模块
     * @param processName 进程名
     * @param moduleName 模块名
     * @return 模块信息
     */
    [[nodiscard]]
    static optional<MODULEENTRY32> searchModule(const std::string& processName,const string& moduleName);

    /*!
     * @brief 通过进程名查找进程ID
     * @param processName 进程名
     * @return 进程ID
     */
    [[nodiscard]]
    static optional<DWORD> searchProcessId(const std::string& processName);

    /*!
     * @brief 通过进程ID获取进程句柄
     * @param processId 进程ID
     * @param access 操作权限
     * @return 进程句柄，失败返回NULL
     */
    [[nodiscard]]
    static HANDLE getProcessHandle(DWORD processId, DWORD access = PROCESS_ALL_ACCESS);

    /*!
     * @brief 通过进程名获取进程句柄
     * @param processName 进程名
     * @return 进程句柄
     */
    [[nodiscard]]
    static HANDLE getProcessHandle(const string& processName);

    /*!
     * @brief 加载进程
     * @param processName 进程名
     * @return 是否成功
     */
    bool loadProcess(const string& processName);

    /*!
     * @brief 获取进程指针
     * @param processName 进程名
     * @return 进程指针
     */
    shared_ptr<Process> getProcess(const string& processName);

private:
    map<string, shared_ptr<Process>> _processMap; // 进程键值表
};
//endregion

