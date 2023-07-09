#include "PlayerHelper.hpp"

PlayerHelper::PlayerHelper() :
_matrixAddr(nullptr),
_angleAddr(nullptr),
_playerAddr(nullptr),
_aimAngle(),
_selfMatrix(),
_roomCount(0)
{
    _pProcess = ProcessHelper::getInstence()->getProcess(setting::processName);
    for (auto& moduleName: setting::moduleGroup)
    {
        _pModule[moduleName] = _pProcess->getModule(moduleName);
    }
	initAddr();
}

bool PlayerHelper::initAddr()
{
	//��ȡ��ַ
	_matrixAddr = (LPVOID)((long) _pModule["client.dll"]->moduleAddr() + setting::offset::client::a_matrix);
	_playerAddr = (LPVOID)((long) _pModule["server.dll"]->moduleAddr() + setting::offset::server::a_player);

	uint64_t readSize = readMemory(_pProcess->processHandle(), (long) _pModule["engine.dll"]->moduleAddr() + setting::offset::engine::a_aim, &_angleAddr, 4);
    if (!readSize)
    {
        warningInfo("��ȡ��Ļ�����ַʧ��");
        return false;
    }

	return true;
}

bool PlayerHelper::updatePlayers()
{
	// ��ȡ��������
	if (!readMemory(_pProcess->processHandle(), (int) _pModule["server.dll"]->moduleAddr() + setting::offset::server::a_playcount, &_roomCount, sizeof(_roomCount)))
		return false;

	// ��ȡ��һ�����Ϣ
	for (uint8_t i = 0; i < _roomCount; i++)
	{
		string name = "player";
		name += std::to_string(i);

		// ����������ʱ��Ϊ��һ�����ʼ�����Լ�
		if (_playerMap.find(name) == _playerMap.end())
		{
			_playerMap[name] = make_shared<player>();
			if (name == "player0")
				_playerMap[name]->isSelf = true;
		}

		// ��ȡ��һ�ַ
		if (!readMemory(_pProcess->processHandle(), (long)_playerAddr + 0x18 * i, &_playerMap[name]->curAddr, 4))
			return false;

		// ��ȡ��Ҿ�����Ϣ
        if (readMemory(_pProcess->processHandle(), (long)_playerMap[name]->curAddr + setting::offset::server::b_camp, &_playerMap[name]->camp, 4) &&
            readMemory(_pProcess->processHandle(), (long)_playerMap[name]->curAddr + setting::offset::server::b_blood, &_playerMap[name]->blood, 4) &&
            readMemory(_pProcess->processHandle(), (long)_playerMap[name]->curAddr + setting::offset::server::b_posX, &_playerMap[name]->location, 12))
            return false;
	}

	return true;
}

bool PlayerHelper::updateAngle()
{
	if (!readMemory(_pProcess->processHandle(), (long)_angleAddr + setting::offset::engine::b_angleY, &_aimAngle, sizeof(float) * 2))
		return false;
	return true;
}

bool PlayerHelper::updateMatrix()
{
	if (!readMemory(_pProcess->processHandle(), _matrixAddr, &_selfMatrix, sizeof(float) * 4 * 4))
        return false;
    return true;
}

bool PlayerHelper::writeAngle(const Point2D& tarAngle)
{
	if (!writeMemory(_pProcess->processHandle(), (long)_angleAddr + setting::offset::engine::b_angleY,
                     tarAngle.floatPoint(), sizeof(float) * 2))
        return false;
    return true;
}

bool PlayerHelper::writeLocation(const string& name, const Point3D& tarLocation)
{
	if (!writeMemory(_pProcess->processHandle(), (long)_playerMap[name]->curAddr + setting::offset::server::b_posX,
                     tarLocation.floatPoint(), sizeof(float) * 3))
        return false;
    return true;
}

bool PlayerHelper::writeBlood(const string& name, size_t& tarBlood)
{
	if (!writeMemory(_pProcess->processHandle(), (int)_playerMap[name]->curAddr + setting::offset::server::b_blood, &tarBlood, sizeof(int)))
        return false;
    return true;
}

// ��ȡ���������Ϣ
unordered_map<string, shared_ptr<player>> PlayerHelper::getPlayers()
{
	return _playerMap;
}

optional<shared_ptr<player>> PlayerHelper::getPlayer(const string& name) noexcept
{
	if (_playerMap.find(name) != _playerMap.end())
		return _playerMap[name];
	return nullopt;
}

// ��ȡ��Ļ����
BaseData::Point2D PlayerHelper::getAimAngle()
{
	return _aimAngle;
}

// ��ȡ��������
float** PlayerHelper::getSelfMatrix()
{
	return (float**)_selfMatrix;
}

void PlayerHelper::printPlayerCon()
{
	printf("�Լ������ַ : 0x%8X \n", _matrixAddr);
	printf("�Լ��ǶȻ�ַ : 0x%8X \n", _angleAddr);
	printf("�����Ϣ��ַ : 0x%8X \n", _playerAddr);
	printf("\n");
}

// ��ӡ��Ҿ�����Ϣ
void PlayerHelper::printPlayer(string playName)
{
	printf("������� : %s \n", playName.c_str());
	printf("��ҵ�ַ : 0x%8X \n", _playerMap[playName]->curAddr);
	printf("�����Ӫ : %d \n", _playerMap[playName]->camp);
	printf("���Ѫ�� : %llu \n", _playerMap[playName]->blood);
	printf("������� : %.4f  %.4f  %.4f \n", _playerMap[playName]->location[0], _playerMap[playName]->location[1], _playerMap[playName]->location[2]);
	cout << endl;
}

void PlayerHelper::printPlayer()
{
	for (auto& player : _playerMap)
	{
		printf("������� : %s \n", player.first.c_str());
		printf("��ҵ�ַ : 0x%8X \n", player.second->curAddr);
		printf("�����Ӫ : %d \n", player.second->camp);
		printf("���Ѫ�� : %llu \n", player.second->blood);
		printf("������� : %.4f  %.4f  %.4f \n", player.second->location[0], player.second->location[1], player.second->location[2]);
		cout << endl;
	}
}

void PlayerHelper::printMatrix()
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

void PlayerHelper::printAngle()
{
	printf("��Ļ���� : %.4f  %.4f\n", _aimAngle[0], _aimAngle[1]);
}

