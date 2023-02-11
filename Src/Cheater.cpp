#include "Cheater.hpp"

Cheater::Cheater()
{
	//加载模块
	auto _pProcess = ProcessController::getInstence();
	_pProcess->loadModule("client.dll");
	_pProcess->loadModule("server.dll");
	_pProcess->loadModule("engine.dll");
	_pProcess->printProcess();
	_pProcess->printModule();
	_pPlayerController = PlayerController::getInstence();
}

Cheater::~Cheater()
{

}

// 计算距离
float Cheater::distance(Point3D& target)
{
	return Point3D::distance(_pPlayerController->getPlayer("player0")->_location, target);
}

// 计算自瞄角度
BaseData::Point2D Cheater::calcuAngle(Point3D& self, Point3D& target)
{
	float x = self.x - target[0];
	float y = self.y - target[1];
	float z = self.z - target[2];

	Point2D aimAngle;
	const float pi = 3.1415f;
	aimAngle.x = (float)atan(z / sqrt(x * x + y * y)) / pi * 180.f;
	aimAngle.y = (float)atan(y / x);

	if (x >= 0.0f && y >= 0.0f) 
		aimAngle[1] = aimAngle[1] / pi * 180.0f - 180.0f;
	else if (x < 0.0f && y >= 0.0f) 
		aimAngle[1] = aimAngle[1] / pi * 180.0f;
	else if (x < 0.0f && y < 0.0f)
		aimAngle[1] = aimAngle[1] / pi * 180.0f;
	else if (x >= 0.0f && y < 0.0f) 
		aimAngle[1] = aimAngle[1] / pi * 180.f + 180.0f;

	return aimAngle;
}

bool Cheater::getMouseL()
{
	return KEY_DOWN(VK_LBUTTON);
}

// 自瞄循环
void Cheater::aimBot()
{
	//更新数据
	_pPlayerController->readPlayer();
	_pPlayerController->readAngle();
	_pPlayerController->readMatrix();

	//获取数据
	auto playerMap = _pPlayerController->getPlayer();
	auto self = _pPlayerController->getPlayer("player0");
	auto selfCamp = self->_camp;

	string target;
	float minDist = 9999;
	for (auto &player: playerMap)
	{
		//自身判断
		if (player.second->_isSelf == true)
			continue;

		//阵营判断
		if (player.second->_camp == selfCamp)
			continue;

		//血量判断
		if (player.second->_blood <= 0 || player.second->_blood > 100)
			continue;

		//计算距离选择最近敌人
		float dist = distance(player.second->_location);
		if (dist < minDist)
		{
			minDist = dist;
			target = player.first;
		}
	}

	//计算自瞄角度
	Point2D aimAngle = calcuAngle(self->_location, playerMap[target]->_location);
	cout << "瞄准角度 ： " << aimAngle << endl;
	printf("瞄准玩家 ：%s\n", target.c_str());
	printf("玩家血量 ：%d\n", playerMap[target]->_blood);
	printf("最小距离 : %f\n", minDist);

	//自瞄角度写入屏幕坐标
	if (getMouseL())
	{
		_pPlayerController->writeAngle(aimAngle);
	}
}

