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
	//读取基址
	_matrixAddr = (LPVOID)((long) _pModule["client.dll"]->moduleAddr() + setting::offset::client::a_matrix);
	_playerAddr = (LPVOID)((long) _pModule["server.dll"]->moduleAddr() + setting::offset::server::a_player);

	uint64_t readSize = readMemory(_pProcess->processHandle(), (long) _pModule["engine.dll"]->moduleAddr() + setting::offset::engine::a_aim, &_angleAddr, 4);
    if (!readSize)
    {
        warningInfo("读取屏幕坐标基址失败");
        return false;
    }

	return true;
}

bool PlayerHelper::updatePlayers()
{
	// 读取房间人数
	if (!readMemory(_pProcess->processHandle(), (int) _pModule["server.dll"]->moduleAddr() + setting::offset::server::a_playcount, &_roomCount, sizeof(_roomCount)))
		return false;

	// 读取玩家基本信息
	for (uint8_t i = 0; i < _roomCount; i++)
	{
		string name = "player";
		name += std::to_string(i);

		// 构建对象，暂时认为第一个玩家始终是自己
		if (_playerMap.find(name) == _playerMap.end())
		{
			_playerMap[name] = make_shared<player>();
			if (name == "player0")
				_playerMap[name]->isSelf = true;
		}

		// 读取玩家基址
		if (!readMemory(_pProcess->processHandle(), (long)_playerAddr + 0x18 * i, &_playerMap[name]->curAddr, 4))
			return false;

		// 读取玩家具体信息
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

// 获取所有玩家信息
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

// 获取屏幕坐标
BaseData::Point2D PlayerHelper::getAimAngle()
{
	return _aimAngle;
}

// 获取世界坐标
float** PlayerHelper::getSelfMatrix()
{
	return (float**)_selfMatrix;
}

void PlayerHelper::printPlayerCon()
{
	printf("自己矩阵基址 : 0x%8X \n", _matrixAddr);
	printf("自己角度基址 : 0x%8X \n", _angleAddr);
	printf("玩家信息基址 : 0x%8X \n", _playerAddr);
	printf("\n");
}

// 打印玩家具体信息
void PlayerHelper::printPlayer(string playName)
{
	printf("玩家名称 : %s \n", playName.c_str());
	printf("玩家地址 : 0x%8X \n", _playerMap[playName]->curAddr);
	printf("玩家阵营 : %d \n", _playerMap[playName]->camp);
	printf("玩家血量 : %llu \n", _playerMap[playName]->blood);
	printf("玩家坐标 : %.4f  %.4f  %.4f \n", _playerMap[playName]->location[0], _playerMap[playName]->location[1], _playerMap[playName]->location[2]);
	cout << endl;
}

void PlayerHelper::printPlayer()
{
	for (auto& player : _playerMap)
	{
		printf("玩家名称 : %s \n", player.first.c_str());
		printf("玩家地址 : 0x%8X \n", player.second->curAddr);
		printf("玩家阵营 : %d \n", player.second->camp);
		printf("玩家血量 : %llu \n", player.second->blood);
		printf("玩家坐标 : %.4f  %.4f  %.4f \n", player.second->location[0], player.second->location[1], player.second->location[2]);
		cout << endl;
	}
}

void PlayerHelper::printMatrix()
{
	cout << "矩阵" << endl;
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
	printf("屏幕坐标 : %.4f  %.4f\n", _aimAngle[0], _aimAngle[1]);
}

