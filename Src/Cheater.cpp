#include "Cheater.hpp"

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

Cheater::Cheater()
{
	try
	{
		//����ģ��
		auto _pProcess = ProcessController::getInstence();
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
Point2D Cheater::worldToScreen(Point3D& self, Point3D& target)
{
	float x = self.x - target[0];
	float y = self.y - target[1];
	float z = self.z - target[2] + 45.0f;

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
		//_pDrawHelper->moveWin();
		////��Ҫ��Ļ�Ƕ�ת��Ϊ��Ļ����
		//Point2D point = worldToScreen(self->_location, player.second->_location);
		////printf("������� ��%s, ���� ��", player.first.c_str());
		////cout << point << endl;
		//_pDrawHelper->startDraw();
		//_pDrawHelper->drawRect(point.x* (float)10, point.y*(float)6, 10, 10, 1, D3DCOLOR_XRGB(122, 255, 0));
		//_pDrawHelper->endDraw();

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
		Point2D aimAngle = worldToScreen(self->_location, playerMap[target]->_location);

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

