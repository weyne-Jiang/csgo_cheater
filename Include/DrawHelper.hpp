#pragma once
#include "Base.hpp"
#include "d3d9.h"
#include "d3dx9.h"
#include "dwmapi.h"
#include "Timer.h"

using namespace BaseData;
using namespace BaseFunc;
using drawFunction = function<void()>;

void drawTest();

class DrawHelper
{
public:

    /*!
     * @brief 创建窗口
     * @param winHand 窗口句柄
     * @return 是否成功
     */
	static bool createWindows(HWND winHand, WNDCLASSEX winCls = transWndCls(WNDCLASSEX(), DrawHelper::WinProc));

    /*!
     * @brief 创建窗口
     * @param className 窗口类名
     * @param windowsName 窗口名
     * @return 是否成功
     */
    static bool createWindows(const string& className, const string& windowsName, WNDCLASSEX winCls = transWndCls(WNDCLASSEX(), DrawHelper::WinProc));

    /*!
     * @brief 注册绘制函数
     * @param drawFunc 绘制函数
     * @return 是否成功
     */
    static bool registerDrawFunc(const drawFunction& drawFunc) noexcept;

    /*!
     * @brief 注销绘制函数
     * @param drawFunc 绘制函数
     * @return 是否成功
     */
    static bool unRegisterDrawFunc(const drawFunction& drawFunc) noexcept;

    /*!
     * @brief 初始化D3D
     * @return 是否成功
     */
    static bool initD3d();

    /*!
     * @brief 消息处理函数
     * @param hWnd 当前窗口句柄
     * @param Message 消息类型
     * @param wParam 左参数
     * @param lParam 右参数
     * @return 结果
     */
	static LRESULT WINAPI WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

    typedef struct LisMsgThPara
    {
        int a = 0;
        char className[256]{};
        char windowsName[256]{};
        WNDCLASSEX wndClass = transWndCls(WNDCLASSEX(), DrawHelper::WinProc);
    } *LPLMThPara;

    static DWORD WINAPI listenMsg(LPVOID lpThreadParameter)
    {
        auto para = static_cast<LPLMThPara>(lpThreadParameter);

        DrawHelper::createWindows(para->className, para->windowsName, para->wndClass);
        DrawHelper::initD3d();
        DrawHelper::registerDrawFunc(drawTest);
        MSG Message;
        ZeroMemory(&Message, sizeof(Message));

        Timer timer;
        uint32_t count = 0;
        timer.start();
        while (GetMessage(&Message, nullptr, 0, 0))
        {
            if (timer.totalTime() > 3*1000)
            {

                cout << "per sec run " << count << " step\n";
                timer.restart();
                count = 0;
            }
            count++;

            moveWin();
            //将消息调度到窗口
            DispatchMessage(&Message);
            //虚拟按键消息转换为字符消息，加入消息队列，便于下次读取
            TranslateMessage(&Message);

        }
        return 0;
    }

    /*!
     * @brief 清理资源
     */
    static void cleanD3d();



	//消息循环
    static void messageLoop();



    /*!
     * @brief 开始绘制
     */
    static void start();

	//同步透明窗口到游戏窗口
	static void moveWin();

	//开始绘制
    static void startDraw();

	//结束绘制
    static void endDraw();

	//绘制线段
    static void drawLine(Point2D& point1, Point2D& point2, float width, D3DCOLOR Color);

	//绘制文字
    static void drawFront(long x, long y, const char* Str, D3DCOLOR Color);

	//绘制矩形
    static void drawRect(float X, float Y, float W, float H, float Width, D3DCOLOR Color);

	//绘制填充矩形
    static void DrawBox(int x, int y, int w, int h, D3DCOLOR Color);

    static inline WNDCLASSEX transWndCls(WNDCLASSEX wClass, WNDPROC WinProc)
    {
        wClass.cbClsExtra = NULL;
        wClass.cbSize = sizeof(WNDCLASSEX);
        wClass.cbWndExtra = NULL;
        wClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
        wClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
        wClass.hInstance = GetModuleHandle(nullptr);
        wClass.lpfnWndProc = WinProc;
        wClass.lpszClassName = "aa";
        wClass.lpszMenuName = " ";
        wClass.style = CS_VREDRAW | CS_HREDRAW;
        return wClass;
    }

//	//获取绘制区域
//	MARGINS getMargin();
//
//	//获取D3D驱动对象指针
//	LPDIRECT3DDEVICE9 getD3dDevice();
//
//	//获取绘制函数
//	drawFunction getDrawFunc();

	DrawHelper() = default;
	~DrawHelper();

private:
	static MARGINS _margin; //绘图区域
	static LPDIRECT3D9 _pD3d; //D3D对象指针
	static LPDIRECT3DDEVICE9 _pD3dDevice;  //D3D驱动对象指针
	static D3DPRESENT_PARAMETERS _d3dPara; // 设备参数
	static ID3DXLine* _pLine; //线段对象指针
	static ID3DXFont* _font; //文字对象指针

	static WNDCLASSEX _wClass;	//注册窗口类
	static HWND _drawHand; //透明窗口绘制句柄
	static HWND _winHwnd; //游戏窗口句柄
	static RECT _windowRect; //窗口边界位置
	static int _windowW; //窗口宽
	static int _windowH; //窗口高

	static drawFunction _drawFunc; //绘制执行函数
    static std::mutex _drawFuncLock; //绘制函数读写锁
    static std::thread _listenThread; //消息监听线程
};
