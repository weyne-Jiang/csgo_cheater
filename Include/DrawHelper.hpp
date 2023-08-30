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
     * @brief ��������
     * @param winHand ���ھ��
     * @return �Ƿ�ɹ�
     */
	static bool createWindows(HWND winHand, WNDCLASSEX winCls = transWndCls(WNDCLASSEX(), DrawHelper::WinProc));

    /*!
     * @brief ��������
     * @param className ��������
     * @param windowsName ������
     * @return �Ƿ�ɹ�
     */
    static bool createWindows(const string& className, const string& windowsName, WNDCLASSEX winCls = transWndCls(WNDCLASSEX(), DrawHelper::WinProc));

    /*!
     * @brief ע����ƺ���
     * @param drawFunc ���ƺ���
     * @return �Ƿ�ɹ�
     */
    static bool registerDrawFunc(const drawFunction& drawFunc) noexcept;

    /*!
     * @brief ע�����ƺ���
     * @param drawFunc ���ƺ���
     * @return �Ƿ�ɹ�
     */
    static bool unRegisterDrawFunc(const drawFunction& drawFunc) noexcept;

    /*!
     * @brief ��ʼ��D3D
     * @return �Ƿ�ɹ�
     */
    static bool initD3d();

    /*!
     * @brief ��Ϣ������
     * @param hWnd ��ǰ���ھ��
     * @param Message ��Ϣ����
     * @param wParam �����
     * @param lParam �Ҳ���
     * @return ���
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
            //����Ϣ���ȵ�����
            DispatchMessage(&Message);
            //���ⰴ����Ϣת��Ϊ�ַ���Ϣ��������Ϣ���У������´ζ�ȡ
            TranslateMessage(&Message);

        }
        return 0;
    }

    /*!
     * @brief ������Դ
     */
    static void cleanD3d();



	//��Ϣѭ��
    static void messageLoop();



    /*!
     * @brief ��ʼ����
     */
    static void start();

	//ͬ��͸�����ڵ���Ϸ����
	static void moveWin();

	//��ʼ����
    static void startDraw();

	//��������
    static void endDraw();

	//�����߶�
    static void drawLine(Point2D& point1, Point2D& point2, float width, D3DCOLOR Color);

	//��������
    static void drawFront(long x, long y, const char* Str, D3DCOLOR Color);

	//���ƾ���
    static void drawRect(float X, float Y, float W, float H, float Width, D3DCOLOR Color);

	//����������
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

//	//��ȡ��������
//	MARGINS getMargin();
//
//	//��ȡD3D��������ָ��
//	LPDIRECT3DDEVICE9 getD3dDevice();
//
//	//��ȡ���ƺ���
//	drawFunction getDrawFunc();

	DrawHelper() = default;
	~DrawHelper();

private:
	static MARGINS _margin; //��ͼ����
	static LPDIRECT3D9 _pD3d; //D3D����ָ��
	static LPDIRECT3DDEVICE9 _pD3dDevice;  //D3D��������ָ��
	static D3DPRESENT_PARAMETERS _d3dPara; // �豸����
	static ID3DXLine* _pLine; //�߶ζ���ָ��
	static ID3DXFont* _font; //���ֶ���ָ��

	static WNDCLASSEX _wClass;	//ע�ᴰ����
	static HWND _drawHand; //͸�����ڻ��ƾ��
	static HWND _winHwnd; //��Ϸ���ھ��
	static RECT _windowRect; //���ڱ߽�λ��
	static int _windowW; //���ڿ�
	static int _windowH; //���ڸ�

	static drawFunction _drawFunc; //����ִ�к���
    static std::mutex _drawFuncLock; //���ƺ�����д��
    static std::thread _listenThread; //��Ϣ�����߳�
};
