#include "Base.hpp"
#include "ProcessHelper.hpp"
#include "PlayerHelper.hpp"
#include "DrawHelper.hpp"
#include "Cheater.hpp"
#include <chrono>
using namespace std::chrono;
//using namespace DrawHelper;
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

int main(int args,char **arges)
{
	////加载模块
	//auto _pProcess = Process::getInstence();
	//_pProcess->loadModule("client.dll");
	//_pProcess->loadModule("server.dll");
	//_pProcess->loadModule("engine.dll");
	//_pProcess->printProcess();
	//_pProcess->printModule();

	//PlayerHelper* ptr1 = PlayerHelper::getInstence();
	//ptr1->updatePlayer();
	//ptr1->updateAngle();
	//ptr1->updateMatrix();
	//ptr1->printMatrix();
	//ptr1->printPlayerCon();
	//ptr1->printPlayer();
	//ptr1->printAngle();
	//ptr1->printMatrix();

	//// 
//	MessageBox(NULL, "开始工作", "提示！", 0);
//	HWND curHwnd = FindWindowA("Direct3DWindowClass", "ShadowVolume");
////	HWND curHwnd = FindWindowA("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
//    auto p = DrawHelper::getInstence();
//    p->createWindows(curHwnd);
//	p->initD3d();
//    p->registerDrawFunc(drawTest);
    DrawHelper::start();
//    CreateThread(NULL, 0, &DrawHelper::test, nullptr, 0, 0);
//    p->messageLoop();
//    p->listenMsg();
//    thread tt(&DrawHelper::messageLoop, &(*p));
    while(1){};
	/*
	int i = 0;
	auto p = Cheater::getInstence();
	auto start = system_clock::now();
	while (1000)
	{
		++i;
		cout << "*********" << "this is " << i << "*************\n";
		p->aimBot();
	}
	auto end = system_clock::now();
	auto cost = duration_cast<milliseconds>(end - start);
	cout << "用时 ： " << double(cost.count()) << "毫秒" << endl;
	system("pause");
	*/

	//auto p1 = DrawHelper::getInstence();
	////HWND curHwnd = FindWindowA("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
	//p1->createWindows("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
	//p1->initD3d();
	//while (1)
	//{
	//	drawTest();
	//}
	//p1->setDrawFunc(drawTest);
	//p1->messageLoop();
//	MyClass1* cls1 = MyClass1::getInstence(3);
//
//	cls1->test();

	return 0;
}