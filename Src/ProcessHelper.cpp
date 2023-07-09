#include "ProcessHelper.hpp"

optional<MODULEENTRY32> ProcessHelper::searchModule(const std::string& processName,const string& moduleName)
{
    auto id = ProcessHelper::searchProcessId(processName);
    if (id.has_value())
        return ProcessHelper::searchModule(id.value(), moduleName);
    else
        return nullopt;
}

optional<MODULEENTRY32> ProcessHelper::searchModule(DWORD processId,const string& moduleName)
{
    //创建快照
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
    if (snap == INVALID_HANDLE_VALUE)
    {
        BaseFunc::warningInfo("CreateToolhelp32Snapshot失败");
        return nullopt;
    }

    MODULEENTRY32 moduleInfo;
    moduleInfo.dwSize = sizeof(MODULEENTRY32);
    //遍历模块
    if (!Module32First(snap, &moduleInfo))
    {
        BaseFunc::warningInfo("Module32First失败" + string("module name: ") + moduleName);
        CloseHandle(snap);
        return nullopt;
    }

    //查找
    do
    {
        if (moduleName == moduleInfo.szModule)
        {
            CloseHandle(snap);
            return moduleInfo;
        }
    } while(Module32Next(snap, &moduleInfo));

    CloseHandle(snap);
    string wainingstr = "未查找到指定模块：" + moduleName;
    BaseFunc::warningInfo("未查找到指定模块");
    return nullopt;
}

optional<DWORD> ProcessHelper::searchProcessId(const string& processName)
{
    //创建快照
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
    {
        BaseFunc::warningInfo("CreateToolhelp32Snapshot失败");
        return nullopt;
    }

    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(PROCESSENTRY32);

    //遍历进程
    if (!Process32First(snap, &processInfo))
    {
        BaseFunc::warningInfo("Process32First失败" + string("process name: ") + processName);
        CloseHandle(snap);
        return nullopt;
    }

    do
    {
        if (processName == processInfo.szExeFile)
        {
            CloseHandle(snap);
            return processInfo.th32ProcessID;
        }
    } while(Process32Next(snap, &processInfo));

    CloseHandle(snap);
    string wainingstr = "未搜寻到进程：" + processName;
    BaseFunc::warningInfo(wainingstr);
    return 0;
}

HANDLE ProcessHelper::getProcessHandle(DWORD processId, DWORD access)
{
    HANDLE processHandle = OpenProcess(access, FALSE, processId);
    if (!processHandle)
        BaseFunc::warningInfo("打开进程" + std::to_string(processId) + "句柄失败");
    return processHandle;
}

HANDLE ProcessHelper::getProcessHandle(const string& processName)
{
    auto id = searchProcessId(processName);
    if (id.has_value())
        return getProcessHandle(id.value());
    else
        return nullptr;
}

bool ProcessHelper::loadProcess(const string& processName)
{
    //判断是否已经载入
    if (_processMap.find(processName) == _processMap.end())
    {
        _processMap[processName] = make_shared<Process>(processName);
        return true;
    }
    else
    {
        BaseFunc::warningInfo("请勿重复加载进程");
        return false;
    }
}

shared_ptr<Process> ProcessHelper::getProcess(const string& processName)
{
    if (_processMap.find(processName) != _processMap.end())
        return _processMap[processName];
    else
    {
        BaseFunc::warningInfo("进程" + processName + "未加载");
        return nullptr;
    }
}

//Process::Process()
//{
//    _processName = "Unknown";
//    _processId = -1;
//    _processHandle = nullptr;
//}

Process::Process(const string& processName)
{
	auto id = ProcessHelper::searchProcessId(processName);
    if (id.has_value())
    {
        _processName = processName;
        _processId = id.value();
        _processHandle = ProcessHelper::getProcessHandle(_processId);
    }
    else
    {
        _processName = "Unknown";
        _processId = -1;
        _processHandle = nullptr;
    }
}

Process::~Process()
{
    CloseHandle(_processHandle);
}

bool Process::loadModule(const string& moduleName)
{
	//判断是否已经载入
	if (_moduleMap.find(moduleName) == _moduleMap.end())
	{
		_moduleMap[moduleName] = make_shared<Module>(_processId, moduleName);
		return true;
	}
	else
	{
		BaseFunc::warningInfo("请勿重复加载模块");
		return false;
	}	
}

shared_ptr<Module> Process::getModule(const string& moduleName) noexcept
{
	if (_moduleMap.find(moduleName) != _moduleMap.end())
		return _moduleMap[moduleName];
	else
	{
        BaseFunc::warningInfo(moduleName + "模块未加载");
		return nullptr;
	}
}

HANDLE Process::processHandle() const noexcept
{
	return _processHandle;
}

DWORD Process::processId() const noexcept
{
	return _processId;
}

string Process::processName() const noexcept
{
    return _processName;
}

void Process::printProcess()
{
	printf("进程名称 : %s \n", _processName.c_str());
	printf("进程ID : %d \n", _processId);
	printf("进程句柄 : 0x%08x \n", _processHandle);
	printf("\n");
}

void Process::printModule()
{
	for (auto &module: _moduleMap)
	{
		module.second->print();
	}
}

Module::Module()
{
    _moduleHandle = nullptr;
    _moduleName = "Unknown";
    _pModuleAddr = nullptr;
    _moduleSize = 0;
    _processId = -1;
    _modulePath = "Unknown";
}

Module::Module(DWORD processId,const string& moduleName)
{
    auto moduleInfo = ProcessHelper::searchModule(processId, moduleName);
    if (moduleInfo.has_value())
    {
        _moduleName = moduleName;
        _pModuleAddr = moduleInfo->modBaseAddr;
        _moduleHandle = moduleInfo->hModule;
        _moduleSize = moduleInfo->modBaseSize;
        _modulePath = moduleInfo->szExePath;
        _processId = moduleInfo->th32ProcessID;
    }
    else
    {
        _moduleHandle = nullptr;
        _moduleName = "Unknown";
        _pModuleAddr = nullptr;
        _moduleSize = 0;
        _processId = -1;
        _modulePath = "Unknown";
    }
}

Module::Module(const string& processName,const string& moduleName)
{
    auto moduleInfo = ProcessHelper::searchModule(processName, moduleName);
    if (moduleInfo.has_value())
    {
        _moduleName = moduleName;
        _pModuleAddr = moduleInfo->modBaseAddr;
        _moduleHandle = moduleInfo->hModule;
        _moduleSize = moduleInfo->modBaseSize;
        _modulePath = moduleInfo->szExePath;
        _processId = moduleInfo->th32ProcessID;
    }
    else
    {
        _moduleHandle = nullptr;
        _moduleName = "Unknown";
        _pModuleAddr = nullptr;
        _moduleSize = 0;
        _processId = -1;
        _modulePath = "Unknown";
    }
}

Module::~Module()
{
	BaseFunc::print("module_died, i haven't thought of anything to do yet.");
}

BYTE* Module::moduleAddr() const noexcept
{
    return _pModuleAddr;
}

void Module::print()
{
	printf("模块名称 : %s \n", _moduleName.c_str());
	printf("模块基址 : 0x%8X \n", _pModuleAddr);
	printf("模块大小 : %d MB\n", _moduleSize / 1024 / 1024);
	printf("\n");
}