#include "Cheater.hpp"

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

Cheater::Cheater()
{
	try
	{
		//����ģ��
		auto _pProcess = Process::getInstence();
		_pProcess->loadModule("client.dll");
		_pProcess->loadModule("server.dll");
		_pProcess->loadModule("engine.dll");
		_pProcess->printProcess();
		_pProcess->printModule();

		//��ʼ����Ҽ�ֵ��
		_pPlayerController = PlayerController::getInstence();

		//��ʼ��͸�����ڡ�D3D����
		_pDrawHelper = DrawHelper::getInstence();
		_pDrawHelper->creatTransWin("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
		_pDrawHelper->initD3d();
	}
	catch (const std::exception& ex)
	{
		string errorInfo = "��ʼ��ʧ�ܣ�����";
		errorInfo += ex.what();
		BaseFunc::errorCheck(errorInfo.c_str());
	}
}

Cheater::~Cheater()
{

}

// �������
float Cheater::distance(Point3D& target)
{
	return Point3D::distance(_pPlayerController->getPlayer("player0")->_location, target);
}

// ��������Ƕ�
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

//ת��Ϊ������Ϣ
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

////������ҷ���
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

// ����ѭ��
void Cheater::aimBot()
{
	//��������
	auto res = _pPlayerController->readPlayer();
	if (res == errorType::readErr)
	{
		cout << "��ȡ����\n";
	}
	_pPlayerController->readAngle();
	_pPlayerController->readMatrix();

	//��ȡ����
	auto playerMap = _pPlayerController->getPlayer();
	auto self = _pPlayerController->getPlayer("player0");
	auto selfCamp = self->_camp;

	string target;
	float minDist = 9999;
	for (auto &player: playerMap)
	{
		//�����ж�
		if (player.second->_isSelf == true)
			continue;

		//��Ӫ�ж�
		if (player.second->_camp == selfCamp)
			continue;

		//Ѫ���ж�
		if (player.second->_blood <= 0 || player.second->_blood > 100)
			continue;

		//����
		//_pDrawHelper->setDrawFunc(drawTest);
		//_pDrawHelper->messageLoop();
		_pDrawHelper->moveWin();
		//��Ҫ��Ļ�Ƕ�ת��Ϊ��Ļ����
		//Point2D point = calcuAngle(self->_location, player.second->_location);
		//printf("������� ��%s, ���� ��", player.first.c_str());
		//cout << point << endl;
		_pDrawHelper->startDraw();
		//_pDrawHelper->drawRect(point.x* (float)10, point.y*(float)6, 10, 10, 1, D3DCOLOR_XRGB(122, 255, 0));
		_pDrawHelper->endDraw();

		//�������ѡ���������
		float dist = distance(player.second->_location);
		if (dist < minDist)
		{
			minDist = dist;
			target = player.first;
		}
	}
	//Point2D aimAngle;
	//��������Ƕ�
	if (target.length() > 0)
	{
		Point2D aimAngle = calcuAngle(self->_location, playerMap[target]->_location);

		//cout << "��׼�Ƕ� �� " << aimAngle << endl;
		//printf("��׼��� ��%s\n", target.c_str());
		//printf("���Ѫ�� ��%d\n", playerMap[target]->_blood);
		//printf("��С���� : %f\n", minDist);

		//����Ƕ�д����Ļ����
		if (getMouseL())
		{
			_pPlayerController->writeAngle(aimAngle);
		}
		//Sleep(100);
	}
}

