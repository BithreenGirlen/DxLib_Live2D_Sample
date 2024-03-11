
#include <Windows.h>
#include <CommCtrl.h>


#include "main_window.h"
#include "win_filesystem.h"
#include "win_dialogue.h"

CMainWindow::CMainWindow()
{

}

CMainWindow::~CMainWindow()
{

}

bool CMainWindow::Create(HINSTANCE hInstance, const wchar_t* pwzWindowName)
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    //wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APP));
    wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = ::GetSysColorBrush(COLOR_BTNFACE);
    //wcex.lpszMenuName = MAKEINTRESOURCE(IDI_ICON_APP);
    wcex.lpszClassName = m_swzClassName;
    //wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_APP));

    if (::RegisterClassExW(&wcex))
    {
        m_hInstance = hInstance;

        m_hWnd = ::CreateWindowW(m_swzClassName, pwzWindowName, WS_OVERLAPPEDWINDOW & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
            CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, nullptr, nullptr, hInstance, this);
        if (m_hWnd != nullptr)
        {
            return true;
        }
        else
        {
            std::wstring wstrMessage = L"CreateWindowExW failed; code: " + std::to_wstring(::GetLastError());
            ::MessageBoxW(nullptr, wstrMessage.c_str(), L"Error", MB_ICONERROR);
        }
    }
    else
    {
        std::wstring wstrMessage = L"RegisterClassW failed; code: " + std::to_wstring(::GetLastError());
        ::MessageBoxW(nullptr, wstrMessage.c_str(), L"Error", MB_ICONERROR);
    }

	return false;
}

int CMainWindow::MessageLoop()
{
    MSG msg;

    for (;;)
    {
        BOOL bRet = ::GetMessageW(&msg, 0, 0, 0);
        if (bRet > 0)
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
        else if (bRet == 0)
        {
            /*ループ終了*/
            return static_cast<int>(msg.wParam);
        }
        else
        {
            /*ループ異常*/
            std::wstring wstrMessage = L"GetMessageW failed; code: " + std::to_wstring(::GetLastError());
            ::MessageBoxW(nullptr, wstrMessage.c_str(), L"Error", MB_ICONERROR);
            return -1;
        }
    }
    return 0;
}
/*C CALLBACK*/
LRESULT CMainWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CMainWindow* pThis = nullptr;
    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = reinterpret_cast<CMainWindow*>(pCreateStruct->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }

    pThis = reinterpret_cast<CMainWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (pThis != nullptr)
    {
        return pThis->HandleMessage(hWnd, uMsg, wParam, lParam);
    }

    return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
/*メッセージ処理*/
LRESULT CMainWindow::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        return OnCreate(hWnd);
    case WM_DESTROY:
        return OnDestroy();
    case WM_CLOSE:
        return OnClose();
    case WM_PAINT:
        return OnPaint();
    case WM_ERASEBKGND:
        return 1;
    case WM_COMMAND:
        return OnCommand(wParam);
    case WM_MOUSEWHEEL:
        return OnMouseWheel(wParam, lParam);
    case WM_LBUTTONDOWN:
        return OnLButtonDown(wParam, lParam);
    case WM_LBUTTONUP:
        return OnLButtonUp(wParam, lParam);
    case WM_MBUTTONUP:
        return OnMButtonUp(wParam, lParam);
    default:
        break;
    }

    return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
/*WM_CREATE*/
LRESULT CMainWindow::OnCreate(HWND hWnd)
{
    m_hWnd = hWnd;

    InitialiseMenuBar();

    m_pDxLibLive2d = new CDxLibLive2d();
    if (m_pDxLibLive2d != nullptr)
    {
        std::wstring wstrDllPath = win_filesystem::GetCurrentProcessPath() + L"\\Live2DCubismCore.dll";
        bool bRet = m_pDxLibLive2d->Setup(m_hWnd, wstrDllPath);
        if (!bRet)
        {
            ::MessageBoxW(nullptr, L"Failed to setup DxLib.", L"Error", MB_ICONERROR);
        }
    }

    return 0;
}
/*WM_DESTROY*/
LRESULT CMainWindow::OnDestroy()
{
    ::PostQuitMessage(0);

    if (m_pDxLibLive2d != nullptr)
    {
        delete m_pDxLibLive2d;
        m_pDxLibLive2d = nullptr;
    }

    return 0;
}
/*WM_CLOSE*/
LRESULT CMainWindow::OnClose()
{
    ::DestroyWindow(m_hWnd);
    ::UnregisterClassW(m_swzClassName, m_hInstance);

    return 0;
}
/*WM_PAINT*/
LRESULT CMainWindow::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint(m_hWnd, &ps);

    if (m_pDxLibLive2d != nullptr)
    {
        m_pDxLibLive2d->Display();
    }

    ::EndPaint(m_hWnd, &ps);

    return 0;
}
/*WM_SIZE*/
LRESULT CMainWindow::OnSize()
{
    return 0;
}
/*WM_COMMAND*/
LRESULT CMainWindow::OnCommand(WPARAM wParam)
{
    int wmKind = HIWORD(wParam);
    int wmId = LOWORD(wParam);
    if (wmKind == 0)
    {
        /*Menus*/
        switch (wmId)
        {
        case Menu::kOpen:
            MenuOnOpen();
            break;
        case Menu::kPauseImage:
            MenuOnPauseImage();
            break;
        case Menu::kSeeThroughImage:
            MenuOnSeeThroughImage();
            break;
        }
    }
    if (wmKind > 1)
    {
        /*Controls*/
    }

    return 0;
}
/*WM_MOUSEWHEEL*/
LRESULT CMainWindow::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
    int iScroll = -static_cast<short>(HIWORD(wParam)) / WHEEL_DELTA;
    WORD wKey = LOWORD(wParam);

    if (wKey == 0 && m_pDxLibLive2d != nullptr)
    {
        m_pDxLibLive2d->RescaleModel(iScroll > 0);
    }

    if (wKey == MK_LBUTTON && m_pDxLibLive2d != nullptr)
    {
        m_pDxLibLive2d->RescaleTime(iScroll > 0);
        m_bSpeedSet = true;
    }

    if (wKey == MK_RBUTTON && m_pDxLibLive2d != nullptr)
    {
        m_pDxLibLive2d->RotateModel(iScroll > 0);
    }

    return 0;
}
/*WM_LBUTTONDOWN*/
LRESULT CMainWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    ::GetCursorPos(&m_CursorPos);

    m_bLeftDowned = true;

    return 0;
}
/*WM_LBUTTONUP*/
LRESULT CMainWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    if (m_bSpeedSet)
    {
        m_bSpeedSet = false;
        return 0;
    }

    WORD usKey = LOWORD(wParam);

    if (usKey == MK_RBUTTON && m_bBarHidden)
    {
        ::PostMessage(m_hWnd, WM_SYSCOMMAND, SC_MOVE, 0);
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = VK_DOWN;
        ::SendInput(1, &input, sizeof(input));
    }

    if (usKey == 0 && m_pDxLibLive2d != nullptr && m_bLeftDowned)
    {
        POINT pt{};
        ::GetCursorPos(&pt);
        int iX = m_CursorPos.x - pt.x;
        int iY = m_CursorPos.y - pt.y;

        if (iX == 0 && iY == 0)
        {
            m_pDxLibLive2d->SwitchMotion();
        }
        else
        {
            /*WM_MOUSEMOVE中での変更は酔うので不採用*/
            m_pDxLibLive2d->SetOffset(iX, iY);
        }
    }

    m_bLeftDowned = false;

    return 0;
}
/*WM_MBUTTONUP*/
LRESULT CMainWindow::OnMButtonUp(WPARAM wParam, LPARAM lParam)
{
    WORD usKey = LOWORD(wParam);

    if (usKey == 0 && m_pDxLibLive2d != nullptr)
    {
        m_pDxLibLive2d->ResetScale();
    }

    if (usKey == MK_RBUTTON)
    {
        SwitchWindowMode();
    }

    return 0;
}
/*操作欄作成*/
void CMainWindow::InitialiseMenuBar()
{
    HMENU hManuFile = nullptr;
    HMENU kMenuImage = nullptr;
    HMENU hMenuBar = nullptr;
    BOOL iRet = FALSE;

    if (m_hMenuBar != nullptr)return;

    hManuFile = ::CreateMenu();
    if (hManuFile == nullptr)goto failed;

    iRet = ::AppendMenuA(hManuFile, MF_STRING, Menu::kOpen, "Open");
    if (iRet == 0)goto failed;

    kMenuImage = ::CreateMenu();
    if (kMenuImage == nullptr)goto failed;

    iRet = ::AppendMenuA(kMenuImage, MF_STRING, Menu::kPauseImage, "Pause");
    if (iRet == 0)goto failed;
    iRet = ::AppendMenuA(kMenuImage, MF_STRING, Menu::kSeeThroughImage, "Through-seen");
    if (iRet == 0)goto failed;

    hMenuBar = ::CreateMenu();
    if (hMenuBar == nullptr) goto failed;

    iRet = ::AppendMenuA(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hManuFile), "File");
    if (iRet == 0)goto failed;
    iRet = ::AppendMenuA(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(kMenuImage), "Image");
    if (iRet == 0)goto failed;

    iRet = ::SetMenu(m_hWnd, hMenuBar);
    if (iRet == 0)goto failed;

    m_hMenuBar = hMenuBar;

    return;

failed:
    std::wstring wstrMessage = L"Failed to create menu; code: " + std::to_wstring(::GetLastError());
    ::MessageBoxW(nullptr, wstrMessage.c_str(), L"Error", MB_ICONERROR);
    if (hManuFile != nullptr)
    {
        ::DestroyMenu(hManuFile);
    }
    if (kMenuImage != nullptr)
    {
        ::DestroyMenu(kMenuImage);
    }
    if (hMenuBar != nullptr)
    {
        ::DestroyMenu(hMenuBar);
    }

}
/*ファイル選択*/
void CMainWindow::MenuOnOpen()
{
    if (m_pDxLibLive2d != nullptr)
    {
        std::wstring wstrPickedFile = win_dialogue::SelectOpenFile(L"model3.json", L"*.model3.json", m_hWnd);
        if (!wstrPickedFile.empty())
        {
            bool bRet = m_pDxLibLive2d->LoadModel(wstrPickedFile.c_str());
            if (!bRet)
            {
                ::MessageBoxW(nullptr, L"Failed to load model.", L"Error", MB_ICONERROR);
            }
            else
            {
                ChangeWindowTitle(wstrPickedFile.c_str());
            }
        }
    }

}
/*一時停止*/
void CMainWindow::MenuOnPauseImage()
{
    if (m_pDxLibLive2d != nullptr)
    {
        HMENU hMenuBar = ::GetMenu(m_hWnd);
        if (hMenuBar != nullptr)
        {
            HMENU hMenu = ::GetSubMenu(hMenuBar, MenuBar::kImage);
            if (hMenu != nullptr)
            {
                bool bRet = m_pDxLibLive2d->SwitchPause();
                ::CheckMenuItem(hMenu, Menu::kPauseImage, bRet ? MF_CHECKED : MF_UNCHECKED);
            }
        }
    }
}
/*透過*/
void CMainWindow::MenuOnSeeThroughImage()
{
    HMENU hMenuBar = ::GetMenu(m_hWnd);
    if (hMenuBar != nullptr)
    {
        HMENU hMenu = ::GetSubMenu(hMenuBar, MenuBar::kImage);
        if (hMenu != nullptr)
        {
            m_bTransparent ^= true;

            LONG lStyleEx = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

            if (m_bTransparent)
            {
                ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyleEx | WS_EX_LAYERED);
                ::SetLayeredWindowAttributes(m_hWnd, RGB(0, 0, 0), 255, LWA_COLORKEY);
                ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            }
            else
            {
                ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyleEx & ~WS_EX_LAYERED);
                ::SetLayeredWindowAttributes(m_hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
                ::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            }

            ::CheckMenuItem(hMenu, Menu::kSeeThroughImage, m_bTransparent ? MF_CHECKED : MF_UNCHECKED);
        }
    }
}
/*標題変更*/
void CMainWindow::ChangeWindowTitle(const wchar_t* pzTitle)
{
    std::wstring wstr;
    if (pzTitle != nullptr)
    {
        std::wstring wstrTitle = pzTitle;
        size_t nPos = wstrTitle.find_last_of(L"\\/");
        wstr = nPos == std::wstring::npos ? wstrTitle : wstrTitle.substr(nPos + 1);
    }

    ::SetWindowTextW(m_hWnd, wstr.c_str());
}
/*表示形式変更*/
void CMainWindow::SwitchWindowMode()
{
    RECT rect;
    ::GetWindowRect(m_hWnd, &rect);
    LONG lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);

    m_bBarHidden ^= true;

    if (m_bBarHidden)
    {
        ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle & ~WS_CAPTION & ~WS_SYSMENU);
        ::SetWindowPos(m_hWnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
        ::SetMenu(m_hWnd, nullptr);
    }
    else
    {
        ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle | WS_CAPTION | WS_SYSMENU);
        ::SetMenu(m_hWnd, m_hMenuBar);
    }

    if (m_pDxLibLive2d != nullptr)
    {
        m_pDxLibLive2d->SwitchSizeLore(m_bBarHidden);
    }
}
