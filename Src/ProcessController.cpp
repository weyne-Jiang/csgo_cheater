#include "ProcessController.hpp"

ProcessController::ProcessController()
{
	//初始化
	_processName = setting::processName;
	_processId = getProcessId(setting::processName);
	_processHandle = getProcessHandle(_processId, PROCESS_ALL_ACCESS);
}

bool ProcessController::loadModule(string moduleName)
{
	//判断是否已经载入
	if (_moduleMap.find(moduleName) == _moduleMap.end())
	{
		_moduleMap[moduleName] = make_shared<ModuleInfo>(_processHandle, _processId, moduleName);
		return true;
	}
	else
	{
		BaseFunc::warningInfo("请勿重复加载模块");
		return false;
	}	
}

shared_ptr<ModuleInfo> ProcessController::getModule(string moduleName)
{
	if (_moduleMap.find(moduleName) != _moduleMap.end())
		return _moduleMap[moduleName];
	else
	{
		printf("%s 模块未加载", moduleName.c_str());
		return nullptr;
	}
}

//通过进程名获取进程ID
DWORD ProcessController::getProcessId(string processName)
{
	//创建快照
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BaseFunc::errorCheck("CreateToolhelp32Snapshot失败", snap != INVALID_HANDLE_VALUE);

	PROCESSENTRY32 processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	processInfo.dwSize = sizeof(processInfo);

	//遍历进程
	bool state = Process32First(snap, &processInfo);
	while (state)
	{
		if (processName == string((char*)processInfo.szExeFile))
		{
			CloseHandle(snap);
			return processInfo.th32ProcessID;
		}
		state = Process32Next(snap, &processInfo);
	}
	CloseHandle(snap);
	string wainingstr = "未搜寻到进程：" + processName;
	BaseFunc::warningInfo(wainingstr.c_str());
	return 0;
}

//通过进程ID获取进程句柄
HANDLE ProcessController::getProcessHandle(DWORD processId, DWORD access)
{
	HANDLE processHandle = OpenProcess(access, FALSE, processId);
	BaseFunc::errorCheck("打开进程句柄失败", processHandle);

	return processHandle;
}

HANDLE ProcessController::getProcessHandle()
{
	return _processHandle;
}

DWORD ProcessController::getProcessId()
{
	return _processId;
}

void ProcessController::printProcess()
{
	printf("进程名称 : %s \n", _processName.c_str());
	printf("进程ID : %d \n", _processId);
	printf("进程句柄 : 0x%08x \n", _processHandle);
	printf("\n");
}

void ProcessController::printModule()
{
	for (auto &module: _moduleMap)
	{
		module.second->print();
	}
}

void ModuleInfo::moduleInit(HANDLE handle, DWORD processId, string moduleName)
{
	//创建快照
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	BaseFunc::errorCheck("CreateToolhelp32Snapshot失败", snap != INVALID_HANDLE_VALUE);

	MODULEENTRY32 moduleInfo;
	ZeroMemory(&moduleInfo, sizeof(moduleInfo));
	moduleInfo.dwSize = sizeof(moduleInfo);

	//遍历模块
	BOOL state = Module32First(snap, &moduleInfo);
	while (state)
	{
		//cout << moduleInfo.szModule << endl;
		if (moduleName == string((char*)moduleInfo.szModule))
		{
			_moduleName = moduleName;
			_pModuleAddr = moduleInfo.modBaseAddr;
			_moduleHandle = moduleInfo.hModule;

			//申请模块内存空间
			//这里需要删除，实际并没用用到
			//_alloc(moduleInfo.modBaseSize);
			//DWORD readSize = BaseFunc::readMemory(handle, _pModuleAddr, _pModuleData, _moduleSize);
			//BaseFunc::errorCheck("读取内存失败", readSize);

			CloseHandle(snap);
			return;
		}
		state = Module32Next(snap, &moduleInfo);
	}

	CloseHandle(snap);
	BaseFunc::warningInfo("未查找到指定模块");
}

ModuleInfo::ModuleInfo(HANDLE handle, DWORD processId, string moduleName) : _moduleName("unknown"), _pModuleAddr(nullptr)
{
	moduleInit(handle, processId, moduleName);
}

ModuleInfo::~ModuleInfo()
{
	_release();
	cout << "module_died, i haven't thought of anything to do yet." << endl;
}

inline void ModuleInfo::_alloc(size_t size)
{
	_moduleSize = size;
	_pModuleData = BaseFunc::allocMemory(size);
}

inline void ModuleInfo::_release()
{
	if (_pModuleData != nullptr)
	{
		BaseFunc::freeMemory(_pModuleData);
	}
}

BYTE* ModuleInfo::getModuleAddr() noexcept
{
	if (nullptr != _pModuleAddr)
		return _pModuleAddr;
	else
		return nullptr;
}

void ModuleInfo::print()
{
	printf("模块名称 : %s \n", _moduleName.c_str());
	printf("模块基址 : 0x%8X \n", _pModuleAddr);
	printf("模块大小 : %d MB\n", _moduleSize / 1024 / 1024);
	printf("\n");
}
