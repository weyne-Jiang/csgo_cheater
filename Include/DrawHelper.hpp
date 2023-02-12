#pragma once
#include "Base.hpp"
#include "d3d9.h"
#include "d3dx9.h"
#include "dwmapi.h"
#include <process.h>

using Point2D = BaseData::Point2D;
using Point3D = BaseData::Point3D;
using drawFunction = function<void()>;

class DrawHelper : public Single<DrawHelper>
{
	friend class Single<DrawHelper>;
public:

	//创建透明窗口
	void creatTransWin(HWND gameHand);

	//创建透明窗口
	void creatTransWin(string className, string windowsName);

	//设置绘制函数
	void setDrawFunc(drawFunction drawFunc);

	//初始化D3D
	bool initD3d();

	//消息处理函数
	static LRESULT WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//清理资源
	void cleanD3d();

	//消息循环
	void messageLoop();

	//同步透明窗口到游戏窗口
	void moveWin();

	//开始绘制
	void startDraw();

	//结束绘制
	void endDraw();

	//绘制线段
	void drawLine(Point2D& point1, Point2D& point2, float width, D3DCOLOR Color);

	//绘制文字
	void drawFront(long x, long y, const char* Str, D3DCOLOR Color);

	//绘制矩形
	void drawRect(float X, float Y, float W, float H, float Width, D3DCOLOR Color);

	//绘制填充矩形
	void DrawBox(int x, int y, int w, int h, D3DCOLOR Color);

	//获取绘制区域
	MARGINS getMargin();

	//获取D3D驱动对象指针
	LPDIRECT3DDEVICE9 getD3dDevice();

	//获取绘制函数
	drawFunction getDrawFunc();

private:
	DrawHelper();
	~DrawHelper();

private:
	MARGINS _margin; //绘图区域
	LPDIRECT3D9 _pD3d = NULL; //D3D对象指针
	LPDIRECT3DDEVICE9 _pD3dDevice = NULL;  //D3D驱动对象指针
	D3DPRESENT_PARAMETERS _d3dpp = {};
	ID3DXLine* _pLine = nullptr; //线段对象指针
	ID3DXFont* _font = nullptr; //文字对象指针

	WNDCLASSEX _wClass;	//注册窗口类
	HWND _drawHand; //绘制句柄
	HWND _gameHwnd; //游戏句柄
	RECT _windowRect; //窗口边界位置
	int _windowW; //窗口宽
	int _windowH; //窗口高

	drawFunction _drawFunc = nullptr; //绘制执行函数
};

