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
	//加载模块
	_pProcess->loadModule("client.dll");
	_pProcess->loadModule("server.dll");
	_pProcess->loadModule("engine.dll");

	//读取基址
	_matrixAddr = (LPVOID)((int)_pProcess->getModule("client.dll")->getModuleAddr()+setting::offset::client::a_matrix);
	_playerAddr = (LPVOID)((int)_pProcess->getModule("server.dll")->getModuleAddr()+setting::offset::server::a_player);

	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_pProcess->getModule("engine.dll")->getModuleAddr() + setting::offset::engine::a_aim), &_angleAddr, 4);
	BaseFunc::errorCheck(readSize, "读取屏幕坐标基址失败");

#ifdef _DEBUG
	printf("自己矩阵基址 : 0x%8X \n", _matrixAddr);
	printf("自己角度基址 : 0x%8X \n", _angleAddr);
	printf("玩家信息基址 : 0x%8X \n", _playerAddr);
	printf("\n");
#endif // _DEBUG

	return true;
}

errorType PlayerController::updatePlayer()
{

	//读取人数
	// 建立键值表
	// 读取单个玩家基址
	// 读取具体信息
	// 若由于玩家退出加入，导致中间地址空挡，解决待定

	// 读取房间人数
	errorType res = successDone;
 	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_pProcess->getModule("server.dll")->getModuleAddr() + setting::offset::server::a_playcount), &_roomCount, sizeof(_roomCount));
	if (0 == readSize)
		return roomCountErr;

	// 读取玩家基本信息
	for (size_t i = 0; i < _roomCount; i++)
	{
		string name = "玩家";
		name += std::to_string(i);
		name += "号";
		// 读取玩家基址
		_playerMap[name] = make_shared<player>();
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerAddr + 0x18*i), &_playerMap[name]->_curAddr, 4);

#ifdef _DEBUG
		printf("玩家地址 : 0x%8X \n", _playerMap[name]->_curAddr);
#endif // _DEBUG
		if (0 == readSize)
		{
			res = playerInfoErr;
			continue;
		}

		// 读取玩家具体信息
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_camp), &_playerMap[name]->_camp, 4);
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_blood), &_playerMap[name]->_blood, 4);
		readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), (LPCVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_posX), &_playerMap[name]->_location, 12);
		if (0 == readSize)
			res = playerInfoErr;

#ifdef _DEBUG
		printf("玩家阵营 : %d \n", _playerMap[name]->_camp);
		printf("玩家血量 : %lu \n", _playerMap[name]->_blood);
		printf("玩家坐标 : %.4f  %.4f  %.4f \n", _playerMap[name]->_location[0], _playerMap[name]->_location[1], _playerMap[name]->_location[2]);
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
	printf("屏幕坐标 : %.4f  %.4f\n", _aimAngle[0], _aimAngle[1]);
#endif // _DEBUG
	return successDone;
}

errorType PlayerController::updateMatrix()
{
	SIZE_T readSize = BaseFunc::readMemory(_pProcess->getProcessHandle(), _matrixAddr, &_selfMatrix, sizeof(float) * 4 * 4);
	//HelpFunc::errorCheck(readSize, "读取engine模块基址失败"); 这个太猛了，别这样
	if (0 == readSize)
		return matrixErr;
	return successDone;
}

void PlayerController::showMatrix()
{
	cout << "世界坐标" << endl;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			printf("%.4f\t", _selfMatrix[i][j]);
		}
		cout << endl;
	}
}

// 写入屏幕坐标
errorType PlayerController::writeAngle(float* tarAngle)
{
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_angleAddr + setting::offset::engine::b_angleY), tarAngle, sizeof(float) * 2);
	if (0 == writeSize)
		return writeErr;
	return successDone;
}

// 写入位置坐标
errorType PlayerController::writeLocation(string name, float* tarLocation)
{
	// 边界判断
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_posX), tarLocation, sizeof(float) * 3);
	if (0 == writeSize)
		return writeErr;
	return successDone;
}

// 写入血量
errorType PlayerController::writeBlood(string name, size_t tarBlood)
{
	// 边界判断
	SIZE_T writeSize = BaseFunc::writeMemory(_pProcess->getProcessHandle(), (LPVOID)((int)_playerMap[name]->_curAddr + setting::offset::server::b_blood), &tarBlood, sizeof(int));
	if (0 == writeSize)
		return writeErr;
	return successDone;
}

// 获取玩家信息
auto PlayerController::getPlayer(string name)
{
	if (_playerMap.find(name) != _playerMap.end())
		return _playerMap[name];
	else
		printf("未找到玩家 %s 信息", name);
	return make_shared<player>();
}

// 获取屏幕坐标
float* PlayerController::getAimAngle()
{
	return _aimAngle;
}

// 获取世界坐标
float** PlayerController::getSelfMatrix()
{
	return (float**)_selfMatrix;
}