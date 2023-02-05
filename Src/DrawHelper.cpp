#include "DrawHelper.hpp"

namespace DrawHelper
{
	//创建透明窗口
	void creatTransWin(HWND gameHand)
	{
		g_gameHwnd = gameHand;

		//初始化窗口类
		g_wClass.cbClsExtra = NULL;
		g_wClass.cbSize = sizeof(WNDCLASSEX);
		g_wClass.cbWndExtra = NULL;
		g_wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
		g_wClass.hCursor = LoadCursor(0, IDC_ARROW);
		g_wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
		g_wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
		g_wClass.hInstance = GetModuleHandle(NULL);
		g_wClass.lpfnWndProc = (WNDPROC)WinProc;
		g_wClass.lpszClassName = " ";
		g_wClass.lpszMenuName = " ";
		g_wClass.style = CS_VREDRAW | CS_HREDRAW;

		//注册窗口
		if (RegisterClassEx(&g_wClass) == 0)
		{
			MessageBox(NULL, "创建窗口出错！", "提示！", 0);
			exit(1);
		}

		//创建窗口
		GetWindowRect(g_gameHwnd, &g_windowRect);
		g_windowW = g_windowRect.right - g_windowRect.left;
		g_windowH = g_windowRect.bottom - g_windowRect.top;
		g_drawHand = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, " ", " ", WS_POPUP, 1, 1, g_windowW, g_windowH, 0, 0, 0, 0);

		//显示窗口
		SetLayeredWindowAttributes(g_drawHand, 0, RGB(0, 0, 0), LWA_COLORKEY);
		ShowWindow(g_drawHand, SW_SHOW);
	}

	//设置绘制函数
	void setDrawFunc(drawFunction drawFunc)
	{
		g_drawFunc = drawFunc;
	}

	//初始化D3D
	bool initD3d()
	{
		// 创建D3D对象
		if ((g_pD3d = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
			return false;

		// 设置D3D参数
		ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
		g_d3dpp.Windowed = TRUE;
		g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		g_d3dpp.EnableAutoDepthStencil = TRUE;
		g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		// 创建D39设备
		if (g_pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_drawHand, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pD3dDevice) < 0)
			return false;

		//创建线段
		if (g_pLine == NULL)
			D3DXCreateLine(g_pD3dDevice, &g_pLine);

		//创建D3D字体
		D3DXCreateFontW(g_pD3dDevice, 16, 0, FW_DONTCARE, D3DX_DEFAULT, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Vernada", &g_font);

		return true;
	}

	//消息处理函数
	LRESULT WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		switch (Message)
		{
		case WM_PAINT:
			if (g_pD3dDevice)
				g_drawFunc();
			break;

		case WM_CREATE:
			DwmExtendFrameIntoClientArea(hWnd, &g_margin);
			break;

		case WM_DESTROY:
		{
			cleanD3d();
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
			cleanD3d();
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);
			break;
		}
		return 0;
	}

	//消息循环
	void messageLoop()
	{
		while (1)
		{
			//固定覆盖游戏窗口
			if (g_gameHwnd)
			{
				GetWindowRect(g_gameHwnd, &g_windowRect);
				g_windowW = g_windowRect.right - g_windowRect.left;
				g_windowH = g_windowRect.bottom - g_windowRect.top;
				printf("windowW : %d, windowH : %d\n", g_windowW, g_windowH);
				DWORD dwStyle = GetWindowLong(g_gameHwnd, GWL_STYLE);
				if (dwStyle & WS_BORDER)
				{
					g_windowRect.left += 8;
					g_windowW += 8;
					g_windowRect.top += 30;
					g_windowH -= 30;
				}
				MoveWindow(g_drawHand, g_windowRect.left, g_windowRect.top, g_windowW, g_windowH, true);
			}

			//处理窗口消息
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

	//清理
	void cleanD3d()
	{
		if (g_pLine)
			g_pLine->Release();
		if (g_font)
			g_font->Release();
		if (g_pD3d)
			g_pD3d->Release();
		if (g_pD3dDevice)
			g_pD3dDevice->Release();
		g_pLine = nullptr;
		g_font = nullptr;
		g_pD3d = nullptr;
		g_pD3dDevice = nullptr;

		CloseWindow(g_drawHand);
		::UnregisterClass(g_wClass.lpszClassName, g_wClass.hInstance);
	}

	//开始绘制
	void startDraw()
	{
		g_pD3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		g_pD3dDevice->BeginScene();
	}

	//结束绘制
	void endDraw()
	{
		g_pD3dDevice->EndScene();
		g_pD3dDevice->Present(0, 0, 0, 0);
	}

	//绘制线段
	void drawLine(float* point1, float* point2, float width, D3DCOLOR Color)
	{
		D3DXVECTOR2 Vertex[2] = { { point1[0], point1[1]}, {point2[0], point2[1]} };

		g_pLine->SetWidth(width);
		g_pLine->Draw(Vertex, 2, Color);
	}

	//绘制文字
	void drawFront(long x, long y, const char* Str, D3DCOLOR Color)
	{
		RECT Rect = { x, y };
		g_font->DrawTextA(NULL, Str, -1, &Rect, DT_CALCRECT, Color);
		g_font->DrawTextA(NULL, Str, -1, &Rect, DT_LEFT, Color);
	}

	//绘制矩形
	void drawRect(float X, float Y, float W, float H, float Width, D3DCOLOR Color)
	{
		D3DXVECTOR2 Vertex[5] = { { X, Y }, { X + W, Y }, { X + W, Y + H }, { X, Y + H }, { X, Y } };
		g_pLine->SetWidth(Width);
		g_pLine->Draw(Vertex, 5, Color);
	}

	//绘制填充矩形
	void DrawBox(int x, int y, int w, int h, D3DCOLOR Color)
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

		g_pD3dDevice->SetTexture(0, NULL);

		// 设置缓冲区格式
		g_pD3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

		// 画两个三角形组成实体矩形
		g_pD3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex));
	}
}
