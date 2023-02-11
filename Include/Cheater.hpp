#pragma once
#include "PlayerController.hpp"
#include "DrawHelper.hpp"

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

using namespace DrawHelper;

class Cheater : public Single<Cheater>
{
	using Point2D = BaseData::Point2D;
	using Point3D = BaseData::Point3D;
    friend class Single<Cheater>;
public:
	// ��ȡ���״̬
	bool getMouseL();

	// �������
	float distance(Point3D& point);

	// ��������Ƕ�
	Point2D calcuAngle(Point3D& self, Point3D& target);

	// ����ѭ��
	void aimBot();

	// 

private:
	Cheater();
    ~Cheater();

private:
	PlayerController* _pPlayerController;

};



