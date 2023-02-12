#include "DrawHelper.hpp"

DrawHelper::DrawHelper()
{

}

DrawHelper::~DrawHelper()
{
}

//����͸������
void DrawHelper::creatTransWin(HWND gameHand)
{
	_gameHwnd = gameHand;

	//��ʼ��������
	_wClass.cbClsExtra = NULL;
	_wClass.cbSize = sizeof(WNDCLASSEX);
	_wClass.cbWndExtra = NULL;
	_wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	_wClass.hCursor = LoadCursor(0, IDC_ARROW);
	_wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	_wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	_wClass.hInstance = GetModuleHandle(NULL);
	_wClass.lpfnWndProc = (WNDPROC)WinProc;
	_wClass.lpszClassName = " ";
	_wClass.lpszMenuName = " ";
	_wClass.style = CS_VREDRAW | CS_HREDRAW;

	//ע�ᴰ��
	if (RegisterClassEx(&_wClass) == 0)
	{
		MessageBox(NULL, "�������ڳ���", "��ʾ��", 0);
		exit(1);
	}

	//��������
	GetWindowRect(_gameHwnd, &_windowRect);
	_windowW = _windowRect.right - _windowRect.left;
	_windowH = _windowRect.bottom - _windowRect.top;
	_drawHand = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, " ", " ", WS_POPUP, 1, 1, _windowW, _windowH, 0, 0, 0, 0);

	//��ʾ����
	SetLayeredWindowAttributes(_drawHand, 0, RGB(0, 0, 0), LWA_COLORKEY);
	ShowWindow(_drawHand, SW_SHOW);
}

//����͸������
void DrawHelper::creatTransWin(string className, string windowsName)
{
	HWND curHwnd = FindWindowA(className.c_str(), windowsName.c_str());
	_gameHwnd = curHwnd;

	//��ʼ��������
	_wClass.cbClsExtra = NULL;
	_wClass.cbSize = sizeof(WNDCLASSEX);
	_wClass.cbWndExtra = NULL;
	_wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	_wClass.hCursor = LoadCursor(0, IDC_ARROW);
	_wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	_wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	_wClass.hInstance = GetModuleHandle(NULL);
	_wClass.lpfnWndProc = (WNDPROC)WinProc;
	_wClass.lpszClassName = " ";
	_wClass.lpszMenuName = " ";
	_wClass.style = CS_VREDRAW | CS_HREDRAW;

	//ע�ᴰ��
	if (RegisterClassEx(&_wClass) == 0)
	{
		MessageBox(NULL, "�������ڳ���", "��ʾ��", 0);
		exit(1);
	}

	//��������
	GetWindowRect(_gameHwnd, &_windowRect);
	_windowW = _windowRect.right - _windowRect.left;
	_windowH = _windowRect.bottom - _windowRect.top;
	_drawHand = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, " ", " ", WS_POPUP, 1, 1, _windowW, _windowH, 0, 0, 0, 0);

	//��ʾ����
	SetLayeredWindowAttributes(_drawHand, 0, RGB(0, 0, 0), LWA_COLORKEY);
	ShowWindow(_drawHand, SW_SHOW);
}

//���û��ƺ���
void DrawHelper::setDrawFunc(drawFunction drawFunc)
{
	_drawFunc = drawFunc;
}

//��ʼ��D3D
bool DrawHelper::initD3d()
{
	// ����D3D����
	if ((_pD3d = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// ����D3D����
	ZeroMemory(&_d3dpp, sizeof(_d3dpp));
	_d3dpp.Windowed = TRUE;
	_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	_d3dpp.EnableAutoDepthStencil = TRUE;
	_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	// ����D39�豸
	if (_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _drawHand, D3DCREATE_HARDWARE_VERTEXPROCESSING, &_d3dpp, &_pD3dDevice) < 0)
		return false;

	//�����߶�
	if (_pLine == NULL)
		D3DXCreateLine(_pD3dDevice, &_pLine);

	//����D3D����
	D3DXCreateFontW(_pD3dDevice, 16, 0, FW_DONTCARE, D3DX_DEFAULT, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Vernada", &_font);

	return true;
}

//��Ա��һ��ָ������ָ�룬�����̬�������÷Ǿ�̬��Ա�����⡣
//��Ϣ������
LRESULT DrawHelper::WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	auto pDraw = DrawHelper::getInstence();
	switch (Message)
	{
	case WM_PAINT:
		if (pDraw->_pD3dDevice)
			pDraw->_drawFunc();
		break;

	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &pDraw->_margin);
		break;

	case WM_DESTROY:
	{
		pDraw->cleanD3d();
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
		pDraw->cleanD3d();
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

//��Ϣѭ��
void DrawHelper::messageLoop()
{
	while (1)
	{
		moveWin();

		//��������Ϣ
		MSG Message;
		ZeroMemory(&Message, sizeof(Message));
		if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&Message);
			TranslateMessage(&Message);
		}
		Sleep(10);
	}
}

//ͬ��͸�����ڵ���Ϸ����
void DrawHelper::moveWin()
{
	//�̶�������Ϸ����
	if (_gameHwnd)
	{
		GetWindowRect(_gameHwnd, &_windowRect);
		_windowW = _windowRect.right - _windowRect.left;
		_windowH = _windowRect.bottom - _windowRect.top;
		//printf("windowW : %d, windowH : %d\n", _windowW, _windowH);
		DWORD dwStyle = GetWindowLong(_gameHwnd, GWL_STYLE);
		if (dwStyle & WS_BORDER)
		{
			_windowRect.left += 8;
			_windowW += 8;
			_windowRect.top += 30;
			_windowH -= 30;
		}
		MoveWindow(_drawHand, _windowRect.left, _windowRect.top, _windowW, _windowH, true);
	}
}

//����
void DrawHelper::cleanD3d()
{
	if (_pLine)
		_pLine->Release();
	if (_font)
		_font->Release();
	if (_pD3d)
		_pD3d->Release();
	if (_pD3dDevice)
		_pD3dDevice->Release();
	_pLine = nullptr;
	_font = nullptr;
	_pD3d = nullptr;
	_pD3dDevice = nullptr;

	CloseWindow(_drawHand);
	::UnregisterClass(_wClass.lpszClassName, _wClass.hInstance);
}

//��ʼ����
void DrawHelper::startDraw()
{
	_pD3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	_pD3dDevice->BeginScene();
}

//��������
void DrawHelper::endDraw()
{
	_pD3dDevice->EndScene();
	_pD3dDevice->Present(0, 0, 0, 0);
}

//�����߶�
void DrawHelper::drawLine(Point2D& point1, Point2D& point2, float width, D3DCOLOR Color)
{
	D3DXVECTOR2 Vertex[2] = { { point1[0], point1[1]}, {point2[0], point2[1]} };

	_pLine->SetWidth(width);
	_pLine->Draw(Vertex, 2, Color);
}

//��������
void DrawHelper::drawFront(long x, long y, const char* Str, D3DCOLOR Color)
{
	RECT Rect = { x, y };
	_font->DrawTextA(NULL, Str, -1, &Rect, DT_CALCRECT, Color);
	_font->DrawTextA(NULL, Str, -1, &Rect, DT_LEFT, Color);
}

//���ƾ���
void DrawHelper::drawRect(float X, float Y, float W, float H, float Width, D3DCOLOR Color)
{
	D3DXVECTOR2 Vertex[5] = { { X, Y }, { X + W, Y }, { X + W, Y + H }, { X, Y + H }, { X, Y } };
	_pLine->SetWidth(Width);
	_pLine->Draw(Vertex, 5, Color);
}

//����������
void DrawHelper::DrawBox(int x, int y, int w, int h, D3DCOLOR Color)
{
	struct Vertex
	{
		float x, y, z, ht;
		DWORD Color;
	};

	Vertex V[8] = { 0 };

	V[0].Color = V[1].Color = V[2].Color = V[3].Color = Color;
	V[0].z = V[1].z = V[2].z = V[3].z = 0.0f;
	V[0].ht = V[1].ht = V[2].ht = V[3].ht = 0.0f;
	V[0].x = V[1].x = (float)x;
	V[0].y = V[2].y = (float)(y + h);
	V[1].y = V[3].y = (float)y;
	V[2].x = V[3].x = (float)(x + w);

	_pD3dDevice->SetTexture(0, NULL);

	// ���û�������ʽ
	_pD3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	// ���������������ʵ�����
	_pD3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex));
}

//��ȡ��������
MARGINS DrawHelper::getMargin()
{
	return _margin;
}

//��ȡD3D��������ָ��
LPDIRECT3DDEVICE9 DrawHelper::getD3dDevice()
{
	return _pD3dDevice;
}

//��ȡ���ƺ���
drawFunction DrawHelper::getDrawFunc()
{
	return _drawFunc;
}