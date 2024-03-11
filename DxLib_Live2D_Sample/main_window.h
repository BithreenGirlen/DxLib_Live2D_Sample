#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <Windows.h>

#include <string>

#include "dxlib_live2d.h"

class CMainWindow
{
public:
	CMainWindow();
	~CMainWindow();
	bool Create(HINSTANCE hInstance, const wchar_t* pwzWindowName);
	int MessageLoop();
	HWND GetHwnd()const { return m_hWnd;}
private:
	const wchar_t* m_swzClassName = L"Dxlib-Live2D window";
	HINSTANCE m_hInstance = nullptr;
	HWND m_hWnd = nullptr;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd);
	LRESULT OnDestroy();
	LRESULT OnClose();
	LRESULT OnPaint();
	LRESULT OnSize();
	LRESULT OnCommand(WPARAM wParam);
	LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMButtonUp(WPARAM wParam, LPARAM lParam);

	enum Menu{kOpen = 1,
		kPauseImage, kSeeThroughImage};
	enum MenuBar{kFile, kImage};

	POINT m_CursorPos{};
	bool m_bSpeedSet = false;
	bool m_bLeftDowned = false;

	HMENU m_hMenuBar = nullptr;
	bool m_bBarHidden = false;
	bool m_bTransparent = false;

	void InitialiseMenuBar();

	void MenuOnOpen();

	void MenuOnPauseImage();
	void MenuOnSeeThroughImage();

	void ChangeWindowTitle(const wchar_t* pzTitle);
	void SwitchWindowMode();

	CDxLibLive2d* m_pDxLibLive2d = nullptr;

};

#endif //MAIN_WINDOW_H_