#include "HelpFunc.hpp"
#include "ProcessController.hpp"
#include "PlayerController.hpp"
#include "DrawHelper.hpp"

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
	float point1[2] = { 200, 300 };
	float point2[2] = { 1, 70 };
	drawLine(point1, point2, 5, D3DCOLOR_XRGB(0, 255, 0));
}

int main(int args,char **arges)
{
	//HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 15304);
	//ProcessController* ptr = ProcessController::getInstence();
	//ptr->loadModule("client.dll");
	//ptr->loadModule("server.dll");
	//ptr->loadModule("engine.dll");
	//PlayerController* ptr2 = PlayerController::getInstence();
	//PlayerController* ptr1 = PlayerController::getInstence();

	//ptr1->updatePlayer();
	//ptr1->updateAngle();
	//
	//ptr1->updateMatrix();
	//ptr1->showMatrix();
	// 
	MessageBox(NULL, L"开始工作", L"提示！", 0);

	HWND curHwnd = FindWindowA("Direct3DWindowClass", "ShadowVolume");
	creatTransWin(curHwnd);
	initD3d();
	setDrawFunc(drawTest);
	messageLoop();


	return 0;
}