#include "Base.hpp"
#include "ProcessController.hpp"
#include "PlayerController.hpp"
#include "DrawHelper.hpp"
#include "Cheater.hpp"
using namespace DrawHelper;
//获取自瞄角度
void get_aimbot_angle(float* self_location, float* player_location, float* aim_angle, bool squat, float recoil)
{
	float x = self_location[0] - player_location[0];
	float y = self_location[1] - player_location[1];
	float z = self_location[2] - player_location[2] + 65.0f;
	if (squat) z -= 15.0f;
	z += recoil;

	const float pi = 3.1415f;
	aim_angle[0] = (float)atan(z / sqrt(x * x + y * y)) / pi * 180.f;
	aim_angle[1] = (float)atan(y / x);

	if (x >= 0.0f && y >= 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f - 180.0f;
	else if (x < 0.0f && y >= 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f;
	else if (x < 0.0f && y < 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f;
	else if (x >= 0.0f && y < 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.f + 180.0f;
}

void drawTest()
{
	startDraw();
	float point1[2] = { 200, 300 };
	float point2[2] = { 1, 70 };
	drawLine(point1, point2, 5, D3DCOLOR_XRGB(122, 255, 0));
	//drawRect(23, 45, 155, 300,567, D3DCOLOR_XRGB(122, 255, 0));
	endDraw();
}

int main(int args,char **arges)
{
	////加载模块
	//auto _pProcess = ProcessController::getInstence();
	//_pProcess->loadModule("client.dll");
	//_pProcess->loadModule("server.dll");
	//_pProcess->loadModule("engine.dll");
	//_pProcess->printProcess();
	//_pProcess->printModule();

	//PlayerController* ptr1 = PlayerController::getInstence();
	//ptr1->readPlayer();
	//ptr1->readAngle();
	//ptr1->readMatrix();
	//ptr1->printMatrix();
	//ptr1->printPlayerCon();
	//ptr1->printPlayer();
	//ptr1->printAngle();
	//ptr1->printMatrix();

	//// 
	//MessageBox(NULL, "开始工作", "提示！", 0);
	////HWND curHwnd = FindWindowA("Direct3DWindowClass", "ShadowVolume");
	//HWND curHwnd = FindWindowA("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
	//creatTransWin(curHwnd);
	//initD3d();
	//setDrawFunc(drawTest);
	//messageLoop();

	auto p = Cheater::getInstence();
	while (true)
	{
		p->aimBot();
	}


	return 0;
}