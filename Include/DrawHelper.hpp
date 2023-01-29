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

static MARGINS g_margin; //��ͼ����
static LPDIRECT3D9 g_pD3d = NULL; //D3D����ָ��
static LPDIRECT3DDEVICE9 g_pD3dDevice = NULL;  //D3D��������ָ��
static D3DPRESENT_PARAMETERS g_d3dpp = {};
static ID3DXLine* g_pLine = nullptr; //�߶ζ���ָ��
static ID3DXFont* g_font = nullptr; //���ֶ���ָ��

static WNDCLASSEX g_wClass;	//ע�ᴰ����
static HWND g_drawHand; //���ƾ��
static HWND g_gameHwnd; //��Ϸ���
static RECT g_windowRect; //���ڱ߽�λ��
static int g_windowW; //���ڿ�
static int g_windowH; //���ڸ�

static drawFunction g_drawFunc;

//����͸������
void creatTransWin(HWND gameHand);

//���û��ƺ���
void setDrawFunc(drawFunction drawFunc);

//��ʼ��D3D
bool initD3d();

//��Ϣ������
LRESULT WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

//������Դ
void cleanD3d();

//��Ϣѭ��
void messageLoop();

//��ʼ����
void startDraw();

//��������
void endDraw();

//�����߶�
void drawLine(float *point1, float *point2, float width, D3DCOLOR Color);

//��������
void drawFront(long x, long y, const char* Str, D3DCOLOR Color);

//���ƾ���
void drawRect(float X, float Y, float W, float H, float Width, D3DCOLOR Color);

//����������
void DrawBox(LPDIRECT3DDEVICE9 pDevice, int x, int y, int w, int h, D3DCOLOR Color);

