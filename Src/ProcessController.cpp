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
	_processId = getProcessId(setting::processName);
	//_processId = 56696;
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
		printf("%s 模块未加载", moduleName.c_str());
		return nullptr;
}

//通过进程名获取进程ID
DWORD ProcessController::getProcessId(string processName)
{
	//创建快照
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BaseFunc::errorCheck(snap != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshot失败");

	PROCESSENTRY32 processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	processInfo.dwSize = sizeof(processInfo);

	//遍历进程
	bool state = Process32First(snap, &processInfo);
	while (state)
	{
		if (processName == string((char*)processInfo.szExeFile))
		{
#ifdef _DEBUG
			printf("进程名称 : %s \n", processInfo.szExeFile);
			printf("进程ID : %d \n", processInfo.th32ProcessID);
			printf("\n");
#endif // DEBUG

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
	BaseFunc::errorCheck(processHandle, "打开进程句柄失败");

#ifdef _DEBUG
	printf("进程ID : %d \n", processId);
	printf("进程句柄 : 0x%08x \n", processHandle);
	printf("\n");
#endif // DEBUG
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

void ModuleInfo::moduleInit(HANDLE handle, DWORD processId, string moduleName)
{
	//创建快照
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	BaseFunc::errorCheck(snap != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshot失败");

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
			_alloc(moduleInfo.modBaseSize);
			DWORD readSize = BaseFunc::readMemory(handle, _pModuleAddr, _pModuleData, _moduleSize);
			BaseFunc::errorCheck(readSize, "读取内存失败");

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
	BaseFunc::freeMemory(_pModuleData);
}

BYTE* ModuleInfo::getModuleAddr() noexcept
{
	if (nullptr != _pModuleAddr)
		return _pModuleAddr;
	else
		return nullptr;
}
