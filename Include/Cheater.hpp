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
	// 获取左键状态
	bool getMouseL();

	// 计算距离
	float distance(Point3D& point);

	// 计算自瞄角度
	Point2D calcuAngle(Point3D& self, Point3D& target);

	// 自瞄循环
	void aimBot();

	// 

private:
	Cheater();
    ~Cheater();

private:
	PlayerController* _pPlayerController;

};



