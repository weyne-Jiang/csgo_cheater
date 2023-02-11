#include "ProcessController.hpp"

ProcessController::ProcessController()
{
	//��ʼ��
	_processName = setting::processName;
	_processId = getProcessId(setting::processName);
	_processHandle = getProcessHandle(_processId, PROCESS_ALL_ACCESS);
}

bool ProcessController::loadModule(string moduleName)
{
	//�ж��Ƿ��Ѿ�����
	if (_moduleMap.find(moduleName) == _moduleMap.end())
	{
		_moduleMap[moduleName] = make_shared<ModuleInfo>(_processHandle, _processId, moduleName);
		return true;
	}
	else
	{
		BaseFunc::warningInfo("�����ظ�����ģ��");
		return false;
	}	
}

shared_ptr<ModuleInfo> ProcessController::getModule(string moduleName)
{
	if (_moduleMap.find(moduleName) != _moduleMap.end())
		return _moduleMap[moduleName];
	else
	{
		printf("%s ģ��δ����", moduleName.c_str());
		return nullptr;
	}
}

//ͨ����������ȡ����ID
DWORD ProcessController::getProcessId(string processName)
{
	//��������
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BaseFunc::errorCheck("CreateToolhelp32Snapshotʧ��", snap != INVALID_HANDLE_VALUE);

	PROCESSENTRY32 processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	processInfo.dwSize = sizeof(processInfo);

	//��������
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
	string wainingstr = "δ��Ѱ�����̣�" + processName;
	BaseFunc::warningInfo(wainingstr.c_str());
	return 0;
}

//ͨ������ID��ȡ���̾��
HANDLE ProcessController::getProcessHandle(DWORD processId, DWORD access)
{
	HANDLE processHandle = OpenProcess(access, FALSE, processId);
	BaseFunc::errorCheck("�򿪽��̾��ʧ��", processHandle);

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
	printf("�������� : %s \n", _processName.c_str());
	printf("����ID : %d \n", _processId);
	printf("���̾�� : 0x%08x \n", _processHandle);
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
	//��������
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	BaseFunc::errorCheck("CreateToolhelp32Snapshotʧ��", snap != INVALID_HANDLE_VALUE);

	MODULEENTRY32 moduleInfo;
	ZeroMemory(&moduleInfo, sizeof(moduleInfo));
	moduleInfo.dwSize = sizeof(moduleInfo);

	//����ģ��
	BOOL state = Module32First(snap, &moduleInfo);
	while (state)
	{
		//cout << moduleInfo.szModule << endl;
		if (moduleName == string((char*)moduleInfo.szModule))
		{
			_moduleName = moduleName;
			_pModuleAddr = moduleInfo.modBaseAddr;
			_moduleHandle = moduleInfo.hModule;

			//����ģ���ڴ�ռ�
			//������Ҫɾ����ʵ�ʲ�û���õ�
			//_alloc(moduleInfo.modBaseSize);
			//DWORD readSize = BaseFunc::readMemory(handle, _pModuleAddr, _pModuleData, _moduleSize);
			//BaseFunc::errorCheck("��ȡ�ڴ�ʧ��", readSize);

			CloseHandle(snap);
			return;
		}
		state = Module32Next(snap, &moduleInfo);
	}

	CloseHandle(snap);
	BaseFunc::warningInfo("δ���ҵ�ָ��ģ��");
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
	printf("ģ������ : %s \n", _moduleName.c_str());
	printf("ģ���ַ : 0x%8X \n", _pModuleAddr);
	printf("ģ���С : %d MB\n", _moduleSize / 1024 / 1024);
	printf("\n");
}
