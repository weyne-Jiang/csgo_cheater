#include "DrawHelper.hpp"
#define DestroySubWin(hand) \
    if (hand)\
    {\
    DestroyWindow(DrawHelper::hand);\
    hand = nullptr;\
    }

MARGINS DrawHelper::_margin{}; //��ͼ����
LPDIRECT3D9 DrawHelper::_pD3d = nullptr; //D3D����ָ��
LPDIRECT3DDEVICE9 DrawHelper::_pD3dDevice = nullptr;  //D3D��������ָ��
D3DPRESENT_PARAMETERS DrawHelper::_d3dPara{}; // �豸����
ID3DXLine* DrawHelper:: _pLine = nullptr; //�߶ζ���ָ��
ID3DXFont* DrawHelper:: _font = nullptr; //���ֶ���ָ��
WNDCLASSEX DrawHelper::_wClass{};	//ע�ᴰ����
HWND DrawHelper::_drawHand = nullptr; //͸�����ڻ��ƾ��
HWND DrawHelper::_winHwnd = nullptr; //��Ϸ���ھ��
RECT DrawHelper::_windowRect{}; //���ڱ߽�λ��
int DrawHelper::_windowW = 0; //���ڿ�
int DrawHelper::_windowH = 0; //���ڸ�
drawFunction DrawHelper::_drawFunc = nullptr; //����ִ�к���
std::thread DrawHelper::_listenThread = thread(); //��Ϣ�����߳�
std::mutex DrawHelper::_drawFuncLock;

void drawTest()
{
    DrawHelper::startDraw();
    Point2D point1(250,250);
    Point2D point2(0, 0);
    DrawHelper::drawLine(point1, point2, 5, D3DCOLOR_XRGB(122, 255, 0));
    //drawRect(23, 45, 155, 300,567, D3DCOLOR_XRGB(122, 255, 0));
    DrawHelper::endDraw();
}

DrawHelper::~DrawHelper()
{
    cleanD3d();
    _listenThread.join();
    //������Ϣ
}

bool DrawHelper::createWindows(HWND winHand, WNDCLASSEX winCls)
{
    _winHwnd = winHand;
	//��ʼ��������
    _wClass = winCls;

	//ע�ᴰ��
	if (!RegisterClassEx(&_wClass))
	{
        warningInfo("ע�ᴰ��ʧ��");
		return false;
	}

	//��������
    if (!GetWindowRect(_winHwnd, &_windowRect))
    {
        warningInfo("��ȡ���ڳߴ�ʧ��");
        return false;
    }
	GetWindowRect(_winHwnd, &_windowRect);
	_windowW = _windowRect.right - _windowRect.left;
	_windowH = _windowRect.bottom - _windowRect.top;
	_drawHand = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, _wClass.lpszClassName, _wClass.lpszMenuName, WS_POPUP, 1, 1, _windowW, _windowH, nullptr, nullptr, nullptr, nullptr);

	//��ʾ����
	SetLayeredWindowAttributes(_drawHand, 0, RGB(0, 0, 0), LWA_COLORKEY);
    ShowWindow(_drawHand, SW_SHOW);
    return true;
}

bool DrawHelper::createWindows(const string& className, const string& windowsName, WNDCLASSEX winCls)
{
	HWND winHwnd = FindWindowA(className.c_str(), windowsName.c_str());
    return createWindows(winHwnd, winCls);
}

//���û��ƺ���
bool DrawHelper::registerDrawFunc(const drawFunction& drawFunc) noexcept
{
    if (!_drawFunc)
    {
        _drawFunc = drawFunc;
        return true;
    }
    return false;
}

bool DrawHelper::unRegisterDrawFunc(const drawFunction& drawFunc) noexcept
{
    if (_drawFunc)
    {
        _drawFunc = nullptr;
        return true;
    }
    return false;
}

//��ʼ��D3D
bool DrawHelper::initD3d()
{
	// ����D3D����
	if (!(_pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
		return false;

	// ����D3D����
	ZeroMemory(&_d3dPara, sizeof(_d3dPara));
    _d3dPara.Windowed = TRUE;
    _d3dPara.SwapEffect = D3DSWAPEFFECT_DISCARD;
    _d3dPara.BackBufferFormat = D3DFMT_UNKNOWN;
    _d3dPara.EnableAutoDepthStencil = TRUE;
    _d3dPara.AutoDepthStencilFormat = D3DFMT_D16;
    _d3dPara.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	// ����D3D9�豸
	if (_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _drawHand, D3DCREATE_HARDWARE_VERTEXPROCESSING, &_d3dPara, &_pD3dDevice) != D3D_OK)
		return false;

	//�����߶�
    if (D3DXCreateLine(_pD3dDevice, &_pLine) != D3D_OK)
        return false;

	//����D3D����
    if (D3DXCreateFontW(_pD3dDevice, 16, 0, FW_DONTCARE, D3DX_DEFAULT, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Vernada", &_font) != S_OK)
        return false;

	return true;
}

LRESULT DrawHelper::WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
        case WM_PAINT:
        {
            std::lock_guard<decltype(_drawFuncLock)> lock(_drawFuncLock);
            if (DrawHelper::_pD3dDevice)
                DrawHelper::_drawFunc();
            break;
        }
        case WM_CREATE:
        {
            if (DwmExtendFrameIntoClientArea(hWnd, &DrawHelper::_margin) != S_OK)
                return -1;
            break;
        }
        case WM_KEYDOWN:
        {
            cout << "WM_KEYDOWN : " << wParam << endl;
            break;
        }
        case WM_CHAR:
        {
            cout << "WM_CHAR : " << wParam << endl;
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_CLOSE:
        {
            DestroySubWin(_drawHand);
            break;
        }
        default:
            return DefWindowProc(hWnd, Message, wParam, lParam);
        }
	return 0;
}

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

    DestroySubWin(_drawHand);
    ::UnregisterClass(_wClass.lpszClassName, _wClass.hInstance);
}

void DrawHelper::start()
{
    LisMsgThPara para;
    strcpy(para.className, "Direct3DWindowClass");
    strcpy(para.windowsName, "ShadowVolume");
    para.a = 666;

    CreateThread(nullptr, 0, &DrawHelper::listenMsg, &para, 0, nullptr);
    //region win-stl compare
//    _listenThread = thread([&](){
//        DrawHelper::createWindows(para.className, para.windowsName, para.wndClass);
//        DrawHelper::initD3d();
//        DrawHelper::registerDrawFunc(drawTest);
//        MSG Message;
//        ZeroMemory(&Message, sizeof(Message));
//
//        Timer timer;
//        uint32_t count = 0;
//        timer.start();
//        while (GetMessage(&Message, nullptr, 0, 0))
//        {
//            if (timer.totalTime() > 3*1000)
//            {
//
//                cout << "per sec run " << count << " step\n";
//                timer.restart();
//                count = 0;
//            }
//            count++;
//
//            moveWin();
//            //����Ϣ���ȵ�����
//            DispatchMessage(&Message);
//            //���ⰴ����Ϣת��Ϊ�ַ���Ϣ��������Ϣ���У������´ζ�ȡ
//            TranslateMessage(&Message);
//
//        }
//    });
    //endregion
}

//��Ϣѭ��
void DrawHelper::messageLoop()
{
	while (true)
	{
		moveWin();

		//��������Ϣ
		MSG Message;
		ZeroMemory(&Message, sizeof(Message));

		if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
            //����Ϣ���ȵ�����
			DispatchMessage(&Message);
            //���ⰴ����Ϣת��Ϊ�ַ���Ϣ��������Ϣ���У������´ζ�ȡ
			TranslateMessage(&Message);
		}
		Sleep(10);
	}
//    BOOL bRet;
//    while( (bRet = GetMessage( &msg, hWnd, 0, 0 )) != 0)
//    {
//        if (bRet == -1)
//        {
//            // handle the error and possibly exit
//        }
//        else
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    }
}

//ͬ��͸�����ڵ���Ϸ����
void DrawHelper::moveWin()
{
	//�̶�������Ϸ����
	if (_winHwnd)
	{
		GetWindowRect(_winHwnd, &_windowRect);
		_windowW = _windowRect.right - _windowRect.left;
		_windowH = _windowRect.bottom - _windowRect.top;
		//printf("windowW : %d, windowH : %d\n", _windowW, _windowH);
		DWORD dwStyle = GetWindowLong(_winHwnd, GWL_STYLE);
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

//��ʼ����
void DrawHelper::startDraw()
{
	_pD3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);
	_pD3dDevice->BeginScene();
}

//��������
void DrawHelper::endDraw()
{
	_pD3dDevice->EndScene();
	_pD3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
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
	_font->DrawTextA(nullptr, Str, -1, &Rect, DT_CALCRECT, Color);
	_font->DrawTextA(nullptr, Str, -1, &Rect, DT_LEFT, Color);
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

	_pD3dDevice->SetTexture(0, nullptr);

	// ���û�������ʽ
	_pD3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	// ���������������ʵ�����
	_pD3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex));
}

////��ȡ��������
//MARGINS DrawHelper::getMargin()
//{
//	return _margin;
//}
//
////��ȡD3D��������ָ��
//LPDIRECT3DDEVICE9 DrawHelper::getD3dDevice()
//{
//	return _pD3dDevice;
//}
//
////��ȡ���ƺ���
//drawFunction DrawHelper::getDrawFunc()
//{
//	return _drawFunc;
//}