#include "PlayerController.hpp"

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
	//��ȡ��ַ
	_matrixAddr = (LPVOID)((int)_pProcess->getModule("client.dll")->getModuleAddr()+setting::offset::client::a_matrix);
	_playerAddr = (LPVOID)((int)_pProcess->getModule("server.dll")->getModuleAddr()+setting::offset::server::a_player);

	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_pProcess->getModule("engine.dll")->getModuleAddr() + setting::offset::engine::a_aim), &_angleAddr, 4);
	BaseFunc::errorCheck("��ȡ��Ļ�����ַʧ��", readSize);

	return true;
}

errorType PlayerController::readPlayer()
{
	// ��ȡ��������
 	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_pProcess->getModule("server.dll")->getModuleAddr() + setting::offset::server::a_playcount), &_roomCount, sizeof(_roomCount));
	if (0 == readSize)
		return errorType::readErr;

	// ��ȡ��һ�����Ϣ
	for (size_t i = 0; i < _roomCount; i++)
	{
		string name = "player";
		name += std::to_string(i);

		// ����������ʱ��Ϊ��һ�����ʼ�����Լ�
		if (_playerMap.find(name) == _playerMap.end())
		{
			_playerMap[name] = make_shared<player>();
			if (name == "player0")
				_playerMap[name]->_isSelf = true;
		}

		// ��ȡ��һ�ַ
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerAddr + 0x18*i), &_playerMap[name]->_curAddr, 4);

		if (0 == readSize)
			return errorType::readErr;

		// ��ȡ��Ҿ�����Ϣ
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_camp), &_playerMap[name]->_camp, 4);
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_blood), &_playerMap[name]->_blood, 4);
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_posX), &_playerMap[name]->_location, 12);
		if (0 == readSize)
			return errorType::readErr;
	}

	return errorType::successDone;
}

errorType PlayerController::readAngle()
{
	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_angleAddr + setting::offset::engine::b_angleY), &_aimAngle, sizeof(float) * 2);

	if (0 == readSize)
		return errorType::readErr;

	return errorType::successDone;
}

errorType PlayerController::readMatrix()
{
	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), _matrixAddr, &_selfMatrix, sizeof(float) * 4 * 4);
	//HelpFunc::errorCheck(readSize, "��ȡengineģ���ַʧ��"); ���̫���ˣ�������
	if (0 == readSize)
		return errorType::readErr;
	return errorType::successDone;
}

// д����Ļ����
errorType PlayerController::writeAngle(Point2D& tarAngle)
{
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_angleAddr + setting::offset::engine::b_angleY), &tarAngle, sizeof(float) * 2);
	if (0 == writeSize)
		return errorType::writeErr;
	return errorType::successDone;
}

// д��λ������
errorType PlayerController::writeLocation(const string& name, Point3D& tarLocation)
{
	// �߽��ж�
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_posX), &tarLocation, sizeof(float) * 3);
	if (0 == writeSize)
		return errorType::writeErr;
	return errorType::successDone;
}

// д��Ѫ��
errorType PlayerController::writeBlood(const string& name, size_t& tarBlood)
{
	// �߽��ж�
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_blood), &tarBlood, sizeof(int));
	if (0 == writeSize)
		return errorType::writeErr;
	return errorType::successDone;
}

// ��ȡ���������Ϣ
unordered_map<string, shared_ptr<player>> PlayerController::getPlayer()
{
	return _playerMap;
}

// ��ȡ�����Ϣ
shared_ptr<player> PlayerController::getPlayer(string name)
{
	if (_playerMap.find(name) != _playerMap.end())
		return _playerMap[name];
	else
		printf("δ�ҵ���� %s ��Ϣ", name.c_str());
	return make_shared<player>();
}

// ��ȡ��Ļ����
BaseData::Point2D PlayerController::getAimAngle()
{
	return _aimAngle;
}

// ��ȡ��������
float** PlayerController::getSelfMatrix()
{
	return (float**)_selfMatrix;
}

void PlayerController::printPlayerCon()
{
	printf("�Լ������ַ : 0x%8X \n", _matrixAddr);
	printf("�Լ��ǶȻ�ַ : 0x%8X \n", _angleAddr);
	printf("�����Ϣ��ַ : 0x%8X \n", _playerAddr);
	printf("\n");
}

// ��ӡ��Ҿ�����Ϣ
void PlayerController::printPlayer(string playName)
{
	printf("������� : %s \n", playName.c_str());
	printf("��ҵ�ַ : 0x%8X \n", _playerMap[playName]->_curAddr);
	printf("�����Ӫ : %d \n", _playerMap[playName]->_camp);
	printf("���Ѫ�� : %llu \n", _playerMap[playName]->_blood);
	printf("������� : %.4f  %.4f  %.4f \n", _playerMap[playName]->_location[0], _playerMap[playName]->_location[1], _playerMap[playName]->_location[2]);
	cout << endl;
}

void PlayerController::printPlayer()
{
	for (auto& player : _playerMap)
	{
		printf("������� : %s \n", player.first.c_str());
		printf("��ҵ�ַ : 0x%8X \n", player.second->_curAddr);
		printf("�����Ӫ : %d \n", player.second->_camp);
		printf("���Ѫ�� : %llu \n", player.second->_blood);
		printf("������� : %.4f  %.4f  %.4f \n", player.second->_location[0], player.second->_location[1], player.second->_location[2]);
		cout << endl;
	}
}

void PlayerController::printMatrix()
{
	cout << "����" << endl;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			printf("%.4f\t", _selfMatrix[i][j]);
		}
		cout << endl;
	}
}

void PlayerController::printAngle()
{
	printf("��Ļ���� : %.4f  %.4f\n", _aimAngle[0], _aimAngle[1]);
}

