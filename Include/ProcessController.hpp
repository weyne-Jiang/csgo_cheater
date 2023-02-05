#pragma once

#include "head.hpp"
#include "HelpFunc.hpp"

class ModuleInfo
{
public:

	ModuleInfo(HANDLE handle, DWORD processId, string moduleName);
	~ModuleInfo();
	// 模块加载初始化
	void moduleInit(HANDLE handle, DWORD processId, string moduleName);
	// 获取模块地址
	BYTE* getModuleAddr() noexcept;
	// 展示指定信息
	void showInfo() = delete;

private:
	//申请模块内存空间
	inline void _alloc(size_t size);
	//释放模块内存空间
	inline void _release();

private:
	// 模块句柄
	HANDLE _moduleHandle;
	string _moduleName;
	// 模块地址
	BYTE* _pModuleAddr;
	// 模块数据存储地址
	LPVOID _pModuleData;
	// 模块尺寸
	size_t _moduleSize;

};

class ProcessController
{
public:
	// 获取进程控制对象指针
	static ProcessController* getInstence();

	// 加载模块
	bool loadModule(string moduleName);

	// 卸载模块
	void unloadModule(string moduleName) = delete;

	// 展示所有进程
	void showAllProcess() = delete;

	// 展示目标进程下所有模块 
	void showAllModule() = delete;

	// 展示指定信息
	void showInfo() = delete;

	//通过进程名获取进程ID
	DWORD getProcessId(string processName);

	//通过进程ID获取进程句柄
	HANDLE getProcessHandle(DWORD processId, DWORD access);

	// 获取进程句柄
	HANDLE getProcessHandle();

	// 获取进程ID
	DWORD getProcessId();

	// 获取模块对象指针
	shared_ptr<ModuleInfo> getModule(string moduleName);

	ProcessController(const ProcessController& cls) = delete;
	ProcessController(ProcessController&& cls) = delete;
	
private:
	//用于自动销毁 RAII
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
	
	//原本使用unique，出现两个问题，一是在外部获取指针时，只能通过move返回，导致指针归属权丢失。
	//二是如果外部调用获取指针，不给予实体对象，只作为匿名对象使用时，指针会原地消失，导致对象被析构。故改为shared_ptr
	map<string, shared_ptr<ModuleInfo>> _moduleMap; // 模块键值表
	HANDLE _processHandle; // 进程句柄
	DWORD _processId; // 进程ID

};

