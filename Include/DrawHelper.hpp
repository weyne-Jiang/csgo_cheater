#pragma once
#include "head.hpp"
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>
#include <process.h>

using drawFunction = function<void()>;

struct DrawData
{
	string test = "nihao";
};

static MARGINS g_margin; //绘图区域
static LPDIRECT3D9 g_pD3d = NULL; //D3D对象指针
static LPDIRECT3DDEVICE9 g_pD3dDevice = NULL;  //D3D驱动对象指针
static D3DPRESENT_PARAMETERS g_d3dpp = {};
static ID3DXLine* g_pLine = nullptr; //线段对象指针
static ID3DXFont* g_font = nullptr; //文字对象指针

static WNDCLASSEX g_wClass;	//注册窗口类
static HWND g_drawHand; //绘制句柄
static HWND g_gameHwnd; //游戏句柄
static RECT g_windowRect; //窗口边界位置
static int g_windowW; //窗口宽
static int g_windowH; //窗口高

static drawFunction g_drawFunc;

//创建透明窗口
void creatTransWin(HWND gameHand);

//设置绘制函数
void setDrawFunc(drawFunction drawFunc);

//初始化D3D
bool initD3d();

//消息处理函数
LRESULT WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

//清理资源
void cleanD3d();

//消息循环
void messageLoop();

//开始绘制
void startDraw();

//结束绘制
void endDraw();

//绘制线段
void drawLine(float *point1, float *point2, float width, D3DCOLOR Color);

//绘制文字
void drawFront(long x, long y, const char* Str, D3DCOLOR Color);

//绘制矩形
void drawRect(float X, float Y, float W, float H, float Width, D3DCOLOR Color);

//绘制填充矩形
void DrawBox(LPDIRECT3DDEVICE9 pDevice, int x, int y, int w, int h, D3DCOLOR Color);

