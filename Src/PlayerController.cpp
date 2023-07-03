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
	//读取基址
	_matrixAddr = (LPVOID)((int)_pProcess->getModule("client.dll")->getModuleAddr()+setting::offset::client::a_matrix);
	_playerAddr = (LPVOID)((int)_pProcess->getModule("server.dll")->getModuleAddr()+setting::offset::server::a_player);

	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_pProcess->getModule("engine.dll")->getModuleAddr() + setting::offset::engine::a_aim), &_angleAddr, 4);
	BaseFunc::errorCheck("读取屏幕坐标基址失败", readSize);

	return true;
}

errorType PlayerController::readPlayer()
{
	// 读取房间人数
 	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_pProcess->getModule("server.dll")->getModuleAddr() + setting::offset::server::a_playcount), &_roomCount, sizeof(_roomCount));
	if (0 == readSize)
		return errorType::readErr;

	// 读取玩家基本信息
	for (size_t i = 0; i < _roomCount; i++)
	{
		string name = "player";
		name += std::to_string(i);

		// 构建对象，暂时认为第一个玩家始终是自己
		if (_playerMap.find(name) == _playerMap.end())
		{
			_playerMap[name] = make_shared<player>();
			if (name == "player0")
				_playerMap[name]->_isSelf = true;
		}

		// 读取玩家基址
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerAddr + 0x18*i), &_playerMap[name]->_curAddr, 4);

		if (0 == readSize)
			return errorType::readErr;

		// 读取玩家具体信息
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
	//HelpFunc::errorCheck(readSize, "读取engine模块基址失败"); 这个太猛了，别这样
	if (0 == readSize)
		return errorType::readErr;
	return errorType::successDone;
}

// 写入屏幕坐标
errorType PlayerController::writeAngle(Point2D& tarAngle)
{
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_angleAddr + setting::offset::engine::b_angleY), &tarAngle, sizeof(float) * 2);
	if (0 == writeSize)
		return errorType::writeErr;
	return errorType::successDone;
}

// 写入位置坐标
errorType PlayerController::writeLocation(const string& name, Point3D& tarLocation)
{
	// 边界判断
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_posX), &tarLocation, sizeof(float) * 3);
	if (0 == writeSize)
		return errorType::writeErr;
	return errorType::successDone;
}

// 写入血量
errorType PlayerController::writeBlood(const string& name, size_t& tarBlood)
{
	// 边界判断
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_blood), &tarBlood, sizeof(int));
	if (0 == writeSize)
		return errorType::writeErr;
	return errorType::successDone;
}

// 获取所有玩家信息
unordered_map<string, shared_ptr<player>> PlayerController::getPlayer()
{
	return _playerMap;
}

// 获取玩家信息
shared_ptr<player> PlayerController::getPlayer(string name)
{
	if (_playerMap.find(name) != _playerMap.end())
		return _playerMap[name];
	else
		printf("未找到玩家 %s 信息", name.c_str());
	return make_shared<player>();
}

// 获取屏幕坐标
BaseData::Point2D PlayerController::getAimAngle()
{
	return _aimAngle;
}

// 获取世界坐标
float** PlayerController::getSelfMatrix()
{
	return (float**)_selfMatrix;
}

void PlayerController::printPlayerCon()
{
	printf("自己矩阵基址 : 0x%8X \n", _matrixAddr);
	printf("自己角度基址 : 0x%8X \n", _angleAddr);
	printf("玩家信息基址 : 0x%8X \n", _playerAddr);
	printf("\n");
}

// 打印玩家具体信息
void PlayerController::printPlayer(string playName)
{
	printf("玩家名称 : %s \n", playName.c_str());
	printf("玩家地址 : 0x%8X \n", _playerMap[playName]->_curAddr);
	printf("玩家阵营 : %d \n", _playerMap[playName]->_camp);
	printf("玩家血量 : %llu \n", _playerMap[playName]->_blood);
	printf("玩家坐标 : %.4f  %.4f  %.4f \n", _playerMap[playName]->_location[0], _playerMap[playName]->_location[1], _playerMap[playName]->_location[2]);
	cout << endl;
}

void PlayerController::printPlayer()
{
	for (auto& player : _playerMap)
	{
		printf("玩家名称 : %s \n", player.first.c_str());
		printf("玩家地址 : 0x%8X \n", player.second->_curAddr);
		printf("玩家阵营 : %d \n", player.second->_camp);
		printf("玩家血量 : %llu \n", player.second->_blood);
		printf("玩家坐标 : %.4f  %.4f  %.4f \n", player.second->_location[0], player.second->_location[1], player.second->_location[2]);
		cout << endl;
	}
}

void PlayerController::printMatrix()
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

void PlayerController::printAngle()
{
	printf("屏幕坐标 : %.4f  %.4f\n", _aimAngle[0], _aimAngle[1]);
}

