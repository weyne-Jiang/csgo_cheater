#pragma once
#include "Base.hpp"
#include "d3d9.h"
#include "d3dx9.h"
#include "dwmapi.h"
#include <process.h>

using namespace BaseData;
using namespace BaseFunc;
using drawFunction = function<void()>;

void drawTest();

class DrawHelper : public Single<DrawHelper>
{
	friend class Single<DrawHelper>;
public:

    /*!
     * @brief 创建窗口
     * @param winHand 窗口句柄
     * @return 是否成功
     */
	bool createWindows(HWND winHand);

    /*!
     * @brief 创建窗口
     * @param className 窗口类名
     * @param windowsName 窗口名
     * @return 是否成功
     */
	bool createWindows(const string& className, const string& windowsName);

    /*!
     * @brief 注册绘制函数
     * @param drawFunc 绘制函数
     * @return 是否成功
     */
	bool registerDrawFunc(const drawFunction& drawFunc) noexcept;

    /*!
     * @brief 注销绘制函数
     * @param drawFunc 绘制函数
     * @return 是否成功
     */
    bool unRegisterDrawFunc(const drawFunction& drawFunc) noexcept;

    /*!
     * @brief 初始化D3D
     * @return 是否成功
     */
	bool initD3d();

    /*!
     * @brief 消息处理函数
     * @param hWnd 当前窗口句柄
     * @param Message 消息类型
     * @param wParam 左参数
     * @param lParam 右参数
     * @return 结果
     */
	static LRESULT WINAPI WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

    static DWORD WINAPI test(LPVOID lpThreadParameter)
    {
        HWND curHwnd = FindWindowA("Direct3DWindowClass", "ShadowVolume");
//	HWND curHwnd = FindWindowA("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
        auto p = DrawHelper::getInstence();
        p->createWindows(curHwnd);
        p->initD3d();
        p->registerDrawFunc(drawTest);
        MSG Message;
        ZeroMemory(&Message, sizeof(Message));
        while (GetMessage(&Message, nullptr, 0, 0))
        {
            moveWin();

            //处理窗口消息
//            if(GetMessage(&Message, nullptr, 0, 0))
//            if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
//            {
                //将消息调度到窗口
                DispatchMessage(&Message);
                //虚拟按键消息转换为字符消息，加入消息队列，便于下次读取
                TranslateMessage(&Message);
//            }
//            Sleep(10);
        }
        return 0;
    }

    /*!
     * @brief 清理资源
     */
	void cleanD3d();

	//消息循环
	void messageLoop();

    /*!
     * @brief 开始绘制
     */
    void listenMsg();

	//同步透明窗口到游戏窗口
	static void moveWin();

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

//	//获取绘制区域
//	MARGINS getMargin();
//
//	//获取D3D驱动对象指针
//	LPDIRECT3DDEVICE9 getD3dDevice();
//
//	//获取绘制函数
//	drawFunction getDrawFunc();

private:
	DrawHelper();
	~DrawHelper() override;

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
    static std::thread _listenThread; //消息监听线程
};
