#include "PlayerController.hpp"

PlayerController* PlayerController::_pInstance = nullptr;
PlayerController::AutoRelease PlayerController::_autoRelease;

PlayerController* PlayerController::getInstence()
{
	if (nullptr == _pInstance)
	{
		_pInstance = new PlayerController();
	}
	return _pInstance;
}

PlayerController::PlayerController() : _matrixAddr(nullptr), _angleAddr(nullptr), _playerAddr(nullptr), _aimAngle(), _selfMatrix()
{
	_pProcess = ProcessController::getInstence();
	initAddr();
}

PlayerController::~PlayerController()
{
}

bool PlayerController::initAddr()
{
	//����ģ��
	_pProcess->loadModule("client.dll");
	_pProcess->loadModule("server.dll");
	_pProcess->loadModule("engine.dll");

	//��ȡ��ַ
	_matrixAddr = (LPVOID)((int)_pProcess->getModule("client.dll")->getModuleAddr()+setting::offset::client::a_matrix);
	_playerAddr = (LPVOID)((int)_pProcess->getModule("server.dll")->getModuleAddr()+setting::offset::server::a_player);

	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_pProcess->getModule("engine.dll")->getModuleAddr() + setting::offset::engine::a_aim), &_angleAddr, 4);
	BaseFunc::errorCheck(readSize, "��ȡ��Ļ�����ַʧ��");

#ifdef _DEBUG
	printf("�Լ������ַ : 0x%8X \n", _matrixAddr);
	printf("�Լ��ǶȻ�ַ : 0x%8X \n", _angleAddr);
	printf("�����Ϣ��ַ : 0x%8X \n", _playerAddr);
	printf("\n");
#endif // _DEBUG

	return true;
}

errorType PlayerController::updatePlayer()
{

	//��ȡ����
	// ������ֵ��
	// ��ȡ������һ�ַ
	// ��ȡ������Ϣ
	// ����������˳����룬�����м��ַ�յ����������

	// ��ȡ��������
	errorType res = successDone;
 	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_pProcess->getModule("server.dll")->getModuleAddr() + setting::offset::server::a_playcount), &_roomCount, sizeof(_roomCount));
	if (0 == readSize)
		return roomCountErr;

	// ��ȡ��һ�����Ϣ
	for (size_t i = 0; i < _roomCount; i++)
	{
		string name = "���";
		name += std::to_string(i);
		name += "��";
		// ��ȡ��һ�ַ
		_playerMap[name] = make_shared<player>();
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerAddr + 0x18*i), &_playerMap[name]->_curAddr, 4);

#ifdef _DEBUG
		printf("��ҵ�ַ : 0x%8X \n", _playerMap[name]->_curAddr);
#endif // _DEBUG
		if (0 == readSize)
		{
			res = playerInfoErr;
			continue;
		}

		// ��ȡ��Ҿ�����Ϣ
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_camp), &_playerMap[name]->_camp, 4);
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_blood), &_playerMap[name]->_blood, 4);
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_posX), &_playerMap[name]->_location, 12);
		if (0 == readSize)
			res = playerInfoErr;

#ifdef _DEBUG
		printf("�����Ӫ : %d \n", _playerMap[name]->_camp);
		printf("���Ѫ�� : %lu \n", _playerMap[name]->_blood);
		printf("������� : %.4f  %.4f  %.4f \n", _playerMap[name]->_location[0], _playerMap[name]->_location[1], _playerMap[name]->_location[2]);
		cout << endl;
#endif // _DEBUG
	}
	return res;
}

errorType PlayerController::updateAngle()
{
	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_angleAddr + setting::offset::engine::b_angleY), &_aimAngle, sizeof(float) * 2);

	if (0 == readSize)
		return angleErr;

#ifdef _DEBUG
	printf("��Ļ���� : %.4f  %.4f\n", _aimAngle[0], _aimAngle[1]);
#endif // _DEBUG
	return successDone;
}

errorType PlayerController::updateMatrix()
{
	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), _matrixAddr, &_selfMatrix, sizeof(float) * 4 * 4);
	//HelpFunc::errorCheck(readSize, "��ȡengineģ���ַʧ��"); ���̫���ˣ�������
	if (0 == readSize)
		return matrixErr;
	return successDone;
}

void PlayerController::showMatrix()
{
	cout << "��������" << endl;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			printf("%.4f\t", _selfMatrix[i][j]);
		}
		cout << endl;
	}
}

// д����Ļ����
errorType PlayerController::writeAngle(float* tarAngle)
{
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_angleAddr + setting::offset::engine::b_angleY), tarAngle, sizeof(float) * 2);
	if (0 == writeSize)
		return writeErr;
	return successDone;
}

// д��λ������
errorType PlayerController::writeLocation(string name, float* tarLocation)
{
	// �߽��ж�
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_posX), tarLocation, sizeof(float) * 3);
	if (0 == writeSize)
		return writeErr;
	return successDone;
}

// д��Ѫ��
errorType PlayerController::writeBlood(string name, size_t tarBlood)
{
	// �߽��ж�
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_blood), &tarBlood, sizeof(int));
	if (0 == writeSize)
		return writeErr;
	return successDone;
}

// ��ȡ�����Ϣ
auto PlayerController::getPlayer(string name)
{
	if (_playerMap.find(name) != _playerMap.end())
		return _playerMap[name];
	else
		printf("δ�ҵ���� %s ��Ϣ", name);
	return make_shared<player>();
}

// ��ȡ��Ļ����
float* PlayerController::getAimAngle()
{
	return _aimAngle;
}

// ��ȡ��������
float** PlayerController::getSelfMatrix()
{
	return (float**)_selfMatrix;
}