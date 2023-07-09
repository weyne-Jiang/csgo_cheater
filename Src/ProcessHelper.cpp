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
    //��������
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
    if (snap == INVALID_HANDLE_VALUE)
    {
        BaseFunc::warningInfo("CreateToolhelp32Snapshotʧ��");
        return nullopt;
    }

    MODULEENTRY32 moduleInfo;
    moduleInfo.dwSize = sizeof(MODULEENTRY32);
    //����ģ��
    if (!Module32First(snap, &moduleInfo))
    {
        BaseFunc::warningInfo("Module32Firstʧ��" + string("module name: ") + moduleName);
        CloseHandle(snap);
        return nullopt;
    }

    //����
    do
    {
        if (moduleName == moduleInfo.szModule)
        {
            CloseHandle(snap);
            return moduleInfo;
        }
    } while(Module32Next(snap, &moduleInfo));

    CloseHandle(snap);
    string wainingstr = "δ���ҵ�ָ��ģ�飺" + moduleName;
    BaseFunc::warningInfo("δ���ҵ�ָ��ģ��");
    return nullopt;
}

optional<DWORD> ProcessHelper::searchProcessId(const string& processName)
{
    //��������
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
    {
        BaseFunc::warningInfo("CreateToolhelp32Snapshotʧ��");
        return nullopt;
    }

    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(PROCESSENTRY32);

    //��������
    if (!Process32First(snap, &processInfo))
    {
        BaseFunc::warningInfo("Process32Firstʧ��" + string("process name: ") + processName);
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
    string wainingstr = "δ��Ѱ�����̣�" + processName;
    BaseFunc::warningInfo(wainingstr);
    return 0;
}

HANDLE ProcessHelper::getProcessHandle(DWORD processId, DWORD access)
{
    HANDLE processHandle = OpenProcess(access, FALSE, processId);
    if (!processHandle)
        BaseFunc::warningInfo("�򿪽���" + std::to_string(processId) + "���ʧ��");
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
    //�ж��Ƿ��Ѿ�����
    if (_processMap.find(processName) == _processMap.end())
    {
        _processMap[processName] = make_shared<Process>(processName);
        return true;
    }
    else
    {
        BaseFunc::warningInfo("�����ظ����ؽ���");
        return false;
    }
}

shared_ptr<Process> ProcessHelper::getProcess(const string& processName)
{
    if (_processMap.find(processName) != _processMap.end())
        return _processMap[processName];
    else
    {
        BaseFunc::warningInfo("����" + processName + "δ����");
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
	//�ж��Ƿ��Ѿ�����
	if (_moduleMap.find(moduleName) == _moduleMap.end())
	{
		_moduleMap[moduleName] = make_shared<Module>(_processId, moduleName);
		return true;
	}
	else
	{
		BaseFunc::warningInfo("�����ظ�����ģ��");
		return false;
	}	
}

shared_ptr<Module> Process::getModule(const string& moduleName) noexcept
{
	if (_moduleMap.find(moduleName) != _moduleMap.end())
		return _moduleMap[moduleName];
	else
	{
        BaseFunc::warningInfo(moduleName + "ģ��δ����");
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
	printf("�������� : %s \n", _processName.c_str());
	printf("����ID : %d \n", _processId);
	printf("���̾�� : 0x%08x \n", _processHandle);
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
	printf("ģ������ : %s \n", _moduleName.c_str());
	printf("ģ���ַ : 0x%8X \n", _pModuleAddr);
	printf("ģ���С : %d MB\n", _moduleSize / 1024 / 1024);
	printf("\n");
}