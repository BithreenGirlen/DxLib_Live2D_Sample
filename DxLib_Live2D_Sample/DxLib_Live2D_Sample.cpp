

#include "framework.h"
#include "DxLib_Live2D_Sample.h"
#include "main_window.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	int iRet = 0;
	CMainWindow* pWindow = new CMainWindow();
	if (pWindow != nullptr)
	{
		bool bRet = pWindow->Create(hInstance, L"DxLib Live2D Sample");
		if (bRet)
		{
			::ShowWindow(pWindow->GetHwnd(), nCmdShow);
			iRet = pWindow->MessageLoop();
		}
		delete pWindow;
	}

	return iRet;
}
