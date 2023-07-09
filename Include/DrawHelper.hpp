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
     * @brief ��������
     * @param winHand ���ھ��
     * @return �Ƿ�ɹ�
     */
	bool createWindows(HWND winHand);

    /*!
     * @brief ��������
     * @param className ��������
     * @param windowsName ������
     * @return �Ƿ�ɹ�
     */
	bool createWindows(const string& className, const string& windowsName);

    /*!
     * @brief ע����ƺ���
     * @param drawFunc ���ƺ���
     * @return �Ƿ�ɹ�
     */
	bool registerDrawFunc(const drawFunction& drawFunc) noexcept;

    /*!
     * @brief ע�����ƺ���
     * @param drawFunc ���ƺ���
     * @return �Ƿ�ɹ�
     */
    bool unRegisterDrawFunc(const drawFunction& drawFunc) noexcept;

    /*!
     * @brief ��ʼ��D3D
     * @return �Ƿ�ɹ�
     */
	bool initD3d();

    /*!
     * @brief ��Ϣ������
     * @param hWnd ��ǰ���ھ��
     * @param Message ��Ϣ����
     * @param wParam �����
     * @param lParam �Ҳ���
     * @return ���
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

            //��������Ϣ
//            if(GetMessage(&Message, nullptr, 0, 0))
//            if (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
//            {
                //����Ϣ���ȵ�����
                DispatchMessage(&Message);
                //���ⰴ����Ϣת��Ϊ�ַ���Ϣ��������Ϣ���У������´ζ�ȡ
                TranslateMessage(&Message);
//            }
//            Sleep(10);
        }
        return 0;
    }

    /*!
     * @brief ������Դ
     */
	void cleanD3d();

	//��Ϣѭ��
	void messageLoop();

    /*!
     * @brief ��ʼ����
     */
    void listenMsg();

	//ͬ��͸�����ڵ���Ϸ����
	static void moveWin();

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

//	//��ȡ��������
//	MARGINS getMargin();
//
//	//��ȡD3D��������ָ��
//	LPDIRECT3DDEVICE9 getD3dDevice();
//
//	//��ȡ���ƺ���
//	drawFunction getDrawFunc();

private:
	DrawHelper();
	~DrawHelper() override;

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
    static std::thread _listenThread; //��Ϣ�����߳�
};
