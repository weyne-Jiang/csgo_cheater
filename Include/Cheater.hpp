#pragma once
#include "PlayerController.hpp"
#include "DrawHelper.hpp"

//using namespace DrawHelper;

class Cheater : public Single<Cheater>
{
    friend class Single<Cheater>;
public:
	// 获取左键状态
	bool getMouseL();

	// 计算距离
	float distance(Point3D& point);

	// 计算自瞄角度
	Point2D worldToScreen(Point3D& self, Point3D& target);

	// 自瞄循环
	void aimBot();

	// 

private:
	Cheater();
    ~Cheater();

private:
	PlayerController* _pPlayerController;
	DrawHelper* _pDrawHelper;
};



