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
	_processId = HelpFunc::getProcessId(setting::processName);
	_processHandle = HelpFunc::getProcessHandle(_processId, PROCESS_ALL_ACCESS);
}

bool ProcessController::loadModule(string moduleName)
{
	//�ж��Ƿ��Ѿ�����
	if (_moduleMap.find(moduleName) == _moduleMap.end())
	{
		_moduleMap[moduleName] = make_unique<ModuleInfo>(_processHandle, _processId, moduleName);
		return true;
	}
	else
	{
		HelpFunc::warningInfo("�����ظ�����ģ��");
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
	HelpFunc::errorCheck(snap != INVALID_HANDLE_VALUE, "CreateToolhelp32Snapshotʧ��");

	MODULEENTRY32 moduleInfo;
	ZeroMemory(&moduleInfo, sizeof(moduleInfo));
	moduleInfo.dwSize = sizeof(moduleInfo);

	//����ģ��
	BOOL state = Module32First(snap, &moduleInfo);
	while (state)
	{
		//cout << string((char*)moduleInfo.szModule) << endl;
		if (moduleName == string((char*)moduleInfo.szModule))
		{
			_moduleName = moduleName;
			_pModuleAddr = moduleInfo.modBaseAddr;
			_moduleHandle = moduleInfo.hModule;

			//����ģ���ڴ�ռ�
			_alloc(moduleInfo.modBaseSize);
			DWORD readSize = HelpFunc::readMemory(handle, _pModuleAddr, _pModuleData, _moduleSize);
			HelpFunc::errorCheck(readSize, "��ȡ�ڴ�ʧ��");

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
	HelpFunc::warningInfo("δ���ҵ�ָ��ģ��");
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
