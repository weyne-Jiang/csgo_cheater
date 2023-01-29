#include "ProcessController.hpp"

ProcessController* ProcessController::_pInstance = nullptr;
ProcessController::AutoRelease ProcessController::_autoRelease;

ProcessController* ProcessController::getInstence()
{
	if (nullptr == _pInstance)
	{
		_pInstance = new ProcessController();
	}
	return _pInstance;
}

ProcessController::ProcessController()
{
	//初始化
	_processId = HelpFunc::getProcessId(setting::processName);
	_processHandle = HelpFunc::getProcessHandle(_processId, PROCESS_ALL_ACCESS);
}

bool ProcessController::loadModule(string moduleName)
{
	//判断是否已经载入
	if (_moduleMap.find(moduleName) == _moduleMap.end())
	{
		_moduleMap[moduleName] = make_unique<ModuleInfo>(_processHandle, _processId, moduleName);
		return true;
	}
	else
	{
		HelpFunc::warningInfo("请勿重复加载模块");
		return false;
	}	
}

shared_ptr<ModuleInfo> ProcessController::getModule(string moduleName)
{
	if (_moduleMap.find(moduleName) != _moduleMap.end())
		return _moduleMap[moduleName];
	else
		printf("%s 模块未加载", moduleName.c_str());
		return nullptr;
}

HANDLE ProcessController::getProcessHandle()
{
	return _processHandle;
}

DWORD ProcessController::getProcessId()
{
	return _processId;
}

void ModuleInfo::moduleInit(HANDLE handle, DWORD processId, string moduleName)
{
	//创建快照
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	HelpFunc::errorCheck(snap != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshot失败");

	MODULEENTRY32 moduleInfo;
	ZeroMemory(&moduleInfo, sizeof(moduleInfo));
	moduleInfo.dwSize = sizeof(moduleInfo);

	//遍历模块
	BOOL state = Module32First(snap, &moduleInfo);
	while (state)
	{
		//cout << string((char*)moduleInfo.szModule) << endl;
		if (moduleName == string((char*)moduleInfo.szModule))
		{
			_moduleName = moduleName;
			_pModuleAddr = moduleInfo.modBaseAddr;
			_moduleHandle = moduleInfo.hModule;

			//申请模块内存空间
			_alloc(moduleInfo.modBaseSize);
			DWORD readSize = HelpFunc::readMemory(handle, _pModuleAddr, _pModuleData, _moduleSize);
			HelpFunc::errorCheck(readSize, "读取内存失败");

#ifdef _DEBUG
			printf("模块名称 : %s \n", _moduleName.c_str());
			printf("模块基址 : 0x%8X \n", _pModuleAddr);
			printf("模块大小 : %d MB\n", _moduleSize / 1024 / 1024);
			printf("实际读取 : %d MB\n", readSize / 1024 / 1024);
			printf("\n");
#endif // DEBUG
			CloseHandle(snap);
			return;
		}
		state = Module32Next(snap, &moduleInfo);
	}

	CloseHandle(snap);
	HelpFunc::warningInfo("未查找到指定模块");
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
	_pModuleData = HelpFunc::allocMemory(size);
}

inline void ModuleInfo::_release()
{
	HelpFunc::freeMemory(_pModuleData);
}

BYTE* ModuleInfo::getModuleAddr() noexcept
{
	if (nullptr != _pModuleAddr)
		return _pModuleAddr;
	else
		return nullptr;
}
