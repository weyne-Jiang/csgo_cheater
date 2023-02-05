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
	//��ʼ��
	_processId = getProcessId(setting::processName);
	//_processId = 56696;
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
		printf("%s ģ��δ����", moduleName.c_str());
		return nullptr;
}

//ͨ����������ȡ����ID
DWORD ProcessController::getProcessId(string processName)
{
	//��������
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BaseFunc::errorCheck(snap != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshotʧ��");

	PROCESSENTRY32 processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	processInfo.dwSize = sizeof(processInfo);

	//��������
	bool state = Process32First(snap, &processInfo);
	while (state)
	{
		if (processName == string((char*)processInfo.szExeFile))
		{
#ifdef _DEBUG
			printf("�������� : %s \n", processInfo.szExeFile);
			printf("����ID : %d \n", processInfo.th32ProcessID);
			printf("\n");
#endif // DEBUG

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
	BaseFunc::errorCheck(processHandle, "�򿪽��̾��ʧ��");

#ifdef _DEBUG
	printf("����ID : %d \n", processId);
	printf("���̾�� : 0x%08x \n", processHandle);
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
	//��������
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
	BaseFunc::errorCheck(snap != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshotʧ��");

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
			_alloc(moduleInfo.modBaseSize);
			DWORD readSize = BaseFunc::readMemory(handle, _pModuleAddr, _pModuleData, _moduleSize);
			BaseFunc::errorCheck(readSize, "��ȡ�ڴ�ʧ��");

#ifdef _DEBUG
			printf("ģ������ : %s \n", _moduleName.c_str());
			printf("ģ���ַ : 0x%8X \n", _pModuleAddr);
			printf("ģ���С : %d MB\n", _moduleSize / 1024 / 1024);
			printf("ʵ�ʶ�ȡ : %d MB\n", readSize / 1024 / 1024);
			printf("\n");
#endif // DEBUG
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
	BaseFunc::freeMemory(_pModuleData);
}

BYTE* ModuleInfo::getModuleAddr() noexcept
{
	if (nullptr != _pModuleAddr)
		return _pModuleAddr;
	else
		return nullptr;
}
