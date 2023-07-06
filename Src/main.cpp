#include "Base.hpp"
#include "ProcessHelper.hpp"
#include "PlayerController.hpp"
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

void drawTest()
{
	auto p = DrawHelper::getInstence();
	p->startDraw();
	Point2D point1(200,300);
	Point2D point2(1, 70);
	p->drawLine(point1, point2, 5, D3DCOLOR_XRGB(122, 255, 0));
	//drawRect(23, 45, 155, 300,567, D3DCOLOR_XRGB(122, 255, 0));
	p->endDraw();
}

//RAII+静态指针变量形式
class MyClass1 : public Single<MyClass1>
{
	//父类需为子类友元，以便能够调用子类私有构造、析构函数
	friend class Single<MyClass1>;
public:
	void test()
	{
		printf("this is test function\n");
	}

private:
	MyClass1() = default;
	MyClass1(int a) { printf("MyClass1(int a), a = %d\n", a); };
	~MyClass1() = default;
};

void const_test1(int i)
{
    i++;
    BaseFunc::print("const_test1", i);
}

void const_test(const int& i)
{
    BaseFunc::print("const_test", i);
    const int a= 1;
    const_test1(a);
}

void throwerr()
{
    throw std::runtime_error("错误");
}

void nothing()
{
    throwerr();
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
	//p1->creatTransWin("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
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
    try
    {
        nothing();
    }
    catch (std::exception ex)
    {
        cout << ex.what() << endl;
    }
    catch (...)
    {
        cout << "sha\n";
    }
	return 0;
}