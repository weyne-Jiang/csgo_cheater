#include "Cheater.hpp"

Cheater::Cheater()
{
	//����ģ��
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

// �������
float Cheater::distance(Point3D& target)
{
	return Point3D::distance(_pPlayerController->getPlayer("player0")->_location, target);
}

// ��������Ƕ�
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

// ����ѭ��
void Cheater::aimBot()
{
	//��������
	_pPlayerController->readPlayer();
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

		//�������ѡ���������
		float dist = distance(player.second->_location);
		if (dist < minDist)
		{
			minDist = dist;
			target = player.first;
		}
	}

	//��������Ƕ�
	Point2D aimAngle = calcuAngle(self->_location, playerMap[target]->_location);
	cout << "��׼�Ƕ� �� " << aimAngle << endl;
	printf("��׼��� ��%s\n", target.c_str());
	printf("���Ѫ�� ��%d\n", playerMap[target]->_blood);
	printf("��С���� : %f\n", minDist);

	//����Ƕ�д����Ļ����
	if (getMouseL())
	{
		_pPlayerController->writeAngle(aimAngle);
	}
}

