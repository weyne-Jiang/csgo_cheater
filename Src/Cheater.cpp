#include "Cheater.hpp"

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

Cheater::Cheater()
{
	try
	{
		//加载模块
		auto _pProcess = Process::getInstence();
		_pProcess->loadModule("client.dll");
		_pProcess->loadModule("server.dll");
		_pProcess->loadModule("engine.dll");
		_pProcess->printProcess();
		_pProcess->printModule();

		//初始化玩家键值表
		_pPlayerController = PlayerController::getInstence();

		//初始化透明窗口、D3D驱动
		_pDrawHelper = DrawHelper::getInstence();
		_pDrawHelper->creatTransWin("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
		_pDrawHelper->initD3d();
	}
	catch (const std::exception& ex)
	{
		string errorInfo = "初始化失败，错误：";
		errorInfo += ex.what();
		BaseFunc::errorCheck(errorInfo.c_str());
	}
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
Point2D Cheater::calcuAngle(Point3D& self, Point3D& target)
{
	float x = self.x() - target[0];
	float y = self.y() - target[1];
	float z = self.z() - target[2] + 45.0f;

	Point2D aimAngle;
	const float pi = 3.1415f;
	aimAngle.x() = (float)atan(z / sqrt(x * x + y * y)) / pi * 180.f;
	aimAngle.y() = (float)atan(y / x);

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

//转化为矩阵信息
bool to_rect_info(float matrix[][4], float* location, int window_width, int window_heigt, int& x, int& y, int& w, int& h)
{
	float to_target = matrix[2][0] * location[0]
		+ matrix[2][1] * location[1]
		+ matrix[2][2] * location[2]
		+ matrix[2][3];
	if (to_target < 0.01f)
	{
		x = y = w = h = 0;
		return false;
	}
	to_target = 1.0f / to_target;

	float to_width = window_width + (matrix[0][0] * location[0]
		+ matrix[0][1] * location[1]
		+ matrix[0][2] * location[2]
		+ matrix[0][3]) * to_target * window_width;

	float to_height_h = window_heigt - (matrix[1][0] * location[0]
		+ matrix[1][1] * location[1]
		+ matrix[1][2] * (location[2] + 75.0f)
		+ matrix[1][3]) * to_target * window_heigt;

	float to_height_w = window_heigt - (matrix[1][0] * location[0]
		+ matrix[1][1] * location[1]
		+ matrix[1][2] * (location[2] - 5.0f)
		+ matrix[1][3]) * to_target * window_heigt;

	x = (int)(to_width - (to_height_w - to_height_h) / 4.0f);
	y = (int)(to_height_h);
	w = (int)((to_height_w - to_height_h) / 2.0f);
	h = (int)(to_height_w - to_height_h);
	return true;
}

////绘制玩家方框
//void render_player_box(player_list* players)
//{
//	int window_x, window_y, window_w, window_h;
//	get_window_size(g_game_hwnd, window_x, window_y, window_w, window_h);
//	window_w /= 2;
//	window_h /= 2;
//
//	float matrix[4][4];
//	read_memory(g_process_handle, g_matrix_address, matrix, sizeof(float) * 4 * 4);
//
//	float self_location[3];
//	get_self_location(self_location);
//
//	int self_camp = get_self_camp(players);
//
//	for (int i = 0; i < g_players_count; i++)
//	{
//		int x, y, w, h;
//		if (players[i].effective && players[i].self == false && to_rect_info(matrix, players[i].location, window_w, window_h, x, y, w, h))
//		{
//			D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 0);
//			if (self_camp != players[i].camp)
//			{
//				color = D3DCOLOR_XRGB(255, 0, 0);
//				players[i].aimbot_len = get_aimbot_len(window_w, window_h, x + (w / 2), y + (h / 2));
//			}
//			render_rect(color, x, y, w, h);
//			render_player_blood(players[i].blood, x - 5, y, h);
//			render_underline(color, window_w, window_h, x + (w / 2), y + h);
//		}
//	}
//}

// 自瞄循环
void Cheater::aimBot()
{
	//更新数据
	auto res = _pPlayerController->readPlayer();
	if (res == errorType::readErr)
	{
		cout << "读取错误\n";
	}
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

		//绘制
		//_pDrawHelper->setDrawFunc(drawTest);
		//_pDrawHelper->messageLoop();
		_pDrawHelper->moveWin();
		//需要屏幕角度转换为屏幕坐标
		//Point2D point = calcuAngle(self->_location, player.second->_location);
		//printf("绘制玩家 ：%s, 坐标 ：", player.first.c_str());
		//cout << point << endl;
		_pDrawHelper->startDraw();
		//_pDrawHelper->drawRect(point.x* (float)10, point.y*(float)6, 10, 10, 1, D3DCOLOR_XRGB(122, 255, 0));
		_pDrawHelper->endDraw();

		//计算距离选择最近敌人
		float dist = distance(player.second->_location);
		if (dist < minDist)
		{
			minDist = dist;
			target = player.first;
		}
	}
	//Point2D aimAngle;
	//计算自瞄角度
	if (target.length() > 0)
	{
		Point2D aimAngle = calcuAngle(self->_location, playerMap[target]->_location);

		//cout << "瞄准角度 ： " << aimAngle << endl;
		//printf("瞄准玩家 ：%s\n", target.c_str());
		//printf("玩家血量 ：%d\n", playerMap[target]->_blood);
		//printf("最小距离 : %f\n", minDist);

		//自瞄角度写入屏幕坐标
		if (getMouseL())
		{
			_pPlayerController->writeAngle(aimAngle);
		}
		//Sleep(100);
	}
}

