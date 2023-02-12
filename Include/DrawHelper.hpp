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

	//����͸������
	void creatTransWin(HWND gameHand);

	//����͸������
	void creatTransWin(string className, string windowsName);

	//���û��ƺ���
	void setDrawFunc(drawFunction drawFunc);

	//��ʼ��D3D
	bool initD3d();

	//��Ϣ������
	static LRESULT WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//������Դ
	void cleanD3d();

	//��Ϣѭ��
	void messageLoop();

	//ͬ��͸�����ڵ���Ϸ����
	void moveWin();

	//��ʼ����
	void startDraw();

	//��������
	void endDraw();

	//�����߶�
	void drawLine(Point2D& point1, Point2D& point2, float width, D3DCOLOR Color);

	//��������
	void drawFront(long x, long y, const char* Str, D3DCOLOR Color);

	//���ƾ���
	void drawRect(float X, float Y, float W, float H, float Width, D3DCOLOR Color);

	//����������
	void DrawBox(int x, int y, int w, int h, D3DCOLOR Color);

	//��ȡ��������
	MARGINS getMargin();

	//��ȡD3D��������ָ��
	LPDIRECT3DDEVICE9 getD3dDevice();

	//��ȡ���ƺ���
	drawFunction getDrawFunc();

private:
	DrawHelper();
	~DrawHelper();

private:
	MARGINS _margin; //��ͼ����
	LPDIRECT3D9 _pD3d = NULL; //D3D����ָ��
	LPDIRECT3DDEVICE9 _pD3dDevice = NULL;  //D3D��������ָ��
	D3DPRESENT_PARAMETERS _d3dpp = {};
	ID3DXLine* _pLine = nullptr; //�߶ζ���ָ��
	ID3DXFont* _font = nullptr; //���ֶ���ָ��

	WNDCLASSEX _wClass;	//ע�ᴰ����
	HWND _drawHand; //���ƾ��
	HWND _gameHwnd; //��Ϸ���
	RECT _windowRect; //���ڱ߽�λ��
	int _windowW; //���ڿ�
	int _windowH; //���ڸ�

	drawFunction _drawFunc = nullptr; //����ִ�к���
};

