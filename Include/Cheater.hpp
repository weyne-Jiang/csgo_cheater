#pragma once
#include "PlayerController.hpp"
#include "DrawHelper.hpp"

//using namespace DrawHelper;

class Cheater : public Single<Cheater>
{
    friend class Single<Cheater>;
public:
	// ��ȡ���״̬
	bool getMouseL();

	// �������
	float distance(Point3D& point);

	// ��������Ƕ�
	Point2D worldToScreen(Point3D& self, Point3D& target);

	// ����ѭ��
	void aimBot();

	// 

private:
	Cheater();
    ~Cheater();

private:
	PlayerController* _pPlayerController;
	DrawHelper* _pDrawHelper;
};



