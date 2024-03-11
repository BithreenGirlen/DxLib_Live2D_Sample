

#include "dxlib_live2d.h"


CDxLibLive2d::CDxLibLive2d()
{

}

CDxLibLive2d::~CDxLibLive2d()
{
	EndThreadpoolTimer();
	DisposeModel();
	
	if (m_iDxLibInitialised != -1)
	{
		DxLib::DxLib_End();
	}
}
/*初期設定*/
bool CDxLibLive2d::Setup(HWND hRenderWnd, const std::wstring &wstrDllPath)
{
	if (m_iDxLibInitialised == 0)return true;

	int iRet = -1;
	iRet = DxLib::SetOutApplicationLogValidFlag(FALSE);
	if (iRet == -1)return false;

	iRet = DxLib::SetUserWindow(hRenderWnd);
	if (iRet == -1)return false;
	m_hRenderWnd = hRenderWnd;

	iRet = DxLib::SetUserWindowMessageProcessDXLibFlag(FALSE);
	if (iRet == -1)return false;

	iRet = DxLib::ChangeWindowMode(TRUE);
	if (iRet == -1)return false;

	iRet = DxLib::SetMultiThreadFlag(TRUE);
	if (iRet == -1)return false;

	iRet = DxLib::SetUseTransColor(FALSE);
	if (iRet == -1)return false;

	iRet = DxLib::SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	if (iRet == -1)return false;

	iRet = DxLib::Live2D_SetCubism4CoreDLLPath(wstrDllPath.c_str());
	if (iRet == -1)return false;

	m_iDxLibInitialised = DxLib::DxLib_Init();
	if (m_iDxLibInitialised == -1)return false;

	iRet = DxLib::SetDrawScreen(DX_SCREEN_BACK);
	if (iRet == -1)
	{
		DxLib::DxLib_End();
		m_iDxLibInitialised = -1;
	}

	return m_iDxLibInitialised != -1;
}
/*読み込み*/
bool CDxLibLive2d::LoadModel(const std::wstring& wstrModelPath)
{
	EndThreadpoolTimer();
	DisposeModel();
	m_iModelHandle = DxLib::Live2D_LoadModel(wstrModelPath.c_str());
	if (m_iModelHandle != -1)
	{
		ClearModelInfo();

		GetExpressionNames();
		GetMotionNames();
		GetHitAreaNames();

		ResetScale();
		StartThreadpoolTimer();
	}
	return m_iModelHandle != -1;
}
/*描画*/
void CDxLibLive2d::Display()
{
	if (m_iModelHandle == -1 || m_motions.empty())return;

	if (DxLib::Live2D_Model_IsMotionFinished(m_iModelHandle) == TRUE)
	{
		DxLib::Live2D_Model_StartMotion(m_iModelHandle, m_motions.at(m_nMotionGroupIndex).wstrMotionGroupName.c_str(), m_iMotionIndex);
	}

	if (!m_bPaused)
	{
		float fDelta = 1 / static_cast<float>(m_nInterval);
		DxLib::Live2D_Model_Update(m_iModelHandle, fDelta);
	}

	DxLib::ClearDrawScreen();
	DxLib::Live2D_RenderBegin();
	DxLib::Live2D_Model_Draw(m_iModelHandle);
	DxLib::Live2D_RenderEnd();
	DxLib::ScreenFlip();
}
/*動作切り替え*/
void CDxLibLive2d::SwitchMotion()
{
	IncrementMotionIndex();
	if (m_iModelHandle != -1 && !m_motions.empty())
	{
		DxLib::Live2D_Model_StartMotion(m_iModelHandle, m_motions.at(m_nMotionGroupIndex).wstrMotionGroupName.c_str(), m_iMotionIndex);
	}
}
/*一時停止切り替え*/
bool CDxLibLive2d::SwitchPause()
{
	m_bPaused ^= true;
	return m_bPaused;
}
/*時間尺度変更*/
void CDxLibLive2d::RescaleTime(bool bHasten)
{
	if (bHasten)
	{
		if (m_nInterval > 1)--m_nInterval;
	}
	else
	{
		++m_nInterval;
	}
}
/*拡縮変更*/
void CDxLibLive2d::RescaleModel(bool bUpscale)
{
	constexpr float fScaleMin = 0.5f;
	if (bUpscale)
	{
		m_fScale += m_fScalePortion;
	}
	else
	{
		m_fScale -= m_fScalePortion;
		if (m_fScale < fScaleMin) m_fScale = fScaleMin;
	}
	UpdateModelScale();
}
/*回転角度変更*/
void CDxLibLive2d::RotateModel(bool bClockWise)
{
	constexpr float fRationPortion = 3.141592f / 22.5f;
	if (bClockWise)
	{
		m_fRotation += fRationPortion;
	}
	else
	{
		m_fRotation -= fRationPortion;
	}
	UpdateModelRotation();
}
/*尺度初期化*/
void CDxLibLive2d::ResetScale()
{
	m_fScale = 1.f;
	m_iXOffset = 0;
	m_iYOffset = 0;
	m_nInterval = Constants::kInterval;
	m_fRotation = 0.f;

	UpdateModelScale();
	UpdateModelOrigin();
	UpdateModelRotation();
}
/*窓枠寸法計算法切り替え*/
void CDxLibLive2d::SwitchSizeLore(bool bBarHidden)
{
	m_bBarHidden = bBarHidden;
	ResizeWindow();
}
/*原点位置移動*/
void CDxLibLive2d::SetOffset(int iX, int iY)
{
	if (m_iModelHandle != -1)
	{
		m_iXOffset -= iX;
		m_iYOffset += iY;
	}
	UpdateModelOrigin();
}
/*モデルに関する取得情報消去*/
void CDxLibLive2d::ClearModelInfo()
{
	m_expressionNames.clear();
	m_nExpressionIndex = 0;

	m_motions.clear();
	m_nMotionGroupIndex = 0;
	m_iMotionIndex = 0;

	m_hitAreaNames.clear();

}
/*表情名称取得*/
void CDxLibLive2d::GetExpressionNames()
{
	if (m_iModelHandle != -1)
	{
		int iSize = DxLib::Live2D_Model_GetExpressionCount(m_iModelHandle);
		if (iSize != -1)
		{
			for (int i = 0; i < iSize; ++i)
			{
				const wchar_t* pBuffer = DxLib::Live2D_Model_GetExpressionName(m_iModelHandle, i);
				if (pBuffer != nullptr)
				{
					m_expressionNames.emplace_back(pBuffer);
				}
			}
		}
	}
}
/*動作名称取得*/
void CDxLibLive2d::GetMotionNames()
{
	if (m_iModelHandle != -1)
	{
		int iSize = DxLib::Live2D_Model_GetMotionGroupCount(m_iModelHandle);
		if (iSize != -1)
		{
			for (int i = 0; i < iSize; ++i)
			{
				const wchar_t* pBuffer = DxLib::Live2D_Model_GetMotionGroupName(m_iModelHandle, i);
				if (pBuffer != nullptr)
				{
					int iCount = DxLib::Live2D_Model_GetMotionCount(m_iModelHandle, pBuffer);
					if (iCount != -1)
					{
						m_motions.emplace_back(dxlib_live2d::SMotion{ pBuffer, iCount });
					}
				}
			}
		}
	}
}
/*当たり部位名称取得*/
void CDxLibLive2d::GetHitAreaNames()
{
	if (m_iModelHandle != -1)
	{
		int iSize = DxLib::Live2D_Model_GetHitAreasCount(m_iModelHandle);
		if (iSize != -1)
		{
			for (int i = 0; i < iSize; ++i)
			{
				const wchar_t* pBuffer = DxLib::Live2D_Model_GetHitAreaName(m_iModelHandle, i);
				if (pBuffer != nullptr)
				{
					m_hitAreaNames.emplace_back(pBuffer);
				}
			}
		}
	}
}

void CDxLibLive2d::GetCamvasSize()
{
	if (m_iModelHandle != -1)
	{
		float fRet = DxLib::Live2D_Model_GetCanvasWidth(m_iModelHandle);
		if (fRet != -1.f)
		{
			m_fCanvasWidth = fRet;
		}
		fRet = DxLib::Live2D_Model_GetCanvasHeight(m_iModelHandle);
		if (fRet != -1.f)
		{
			m_fCanvasHeight = fRet;
		}
	}
}
/*動作番号逓加*/
void CDxLibLive2d::IncrementMotionIndex()
{
	if (!m_motions.empty())
	{
		++m_iMotionIndex;
		/*大きさではなく識別番号*/
		if (m_iMotionIndex > m_motions.at(m_nMotionGroupIndex).iLargestMotionId)
		{
			m_iMotionIndex = 0;
			++m_nMotionGroupIndex;
			if (m_nMotionGroupIndex >= m_motions.size())
			{
				m_nMotionGroupIndex = 0;
			}
		}
	}
}
/*破棄*/
void CDxLibLive2d::DisposeModel()
{
	if (m_iModelHandle != -1)
	{
		DxLib::Live2D_DeleteModel(m_iModelHandle);
		m_iModelHandle = -1;
	}
}
/*拡縮適用*/
void CDxLibLive2d::UpdateModelScale()
{
	if (m_iModelHandle != -1)
	{
		DxLib::Live2D_Model_SetExtendRate(m_iModelHandle, m_fScale, m_fScale);
	}
	ResizeWindow();
}
/*原点位置適用*/
void CDxLibLive2d::UpdateModelOrigin()
{
	if (m_iModelHandle != -1)
	{
		DxLib::Live2D_Model_SetTranslate(m_iModelHandle, static_cast<float>(m_iXOffset), static_cast<float>(m_iYOffset));
	}
}
/*回転角度適用*/
void CDxLibLive2d::UpdateModelRotation()
{
	if (m_iModelHandle != -1)
	{
		DxLib::Live2D_Model_SetRotate(m_iModelHandle, m_fRotation);
	}
}
/*窓寸法調整*/
void CDxLibLive2d::ResizeWindow()
{
	if (m_hRenderWnd != nullptr)
	{
		RECT rect;
		if (!m_bBarHidden)
		{
			::GetWindowRect(m_hRenderWnd, &rect);
		}
		else
		{
			::GetClientRect(m_hRenderWnd, &rect);
		}

		float fDpiScale = ::GetDpiForWindow(m_hRenderWnd) / 96.f;
		int iX = static_cast<int>(Constants::kBaseWidth * m_fScale * fDpiScale);
		int iY = static_cast<int>(Constants::kBaseHeight * m_fScale * fDpiScale);
		rect.right = iX + rect.left;
		rect.bottom = iY + rect.top;
		if (!m_bBarHidden)
		{
			LONG lStyle = ::GetWindowLong(m_hRenderWnd, GWL_STYLE);
			::AdjustWindowRect(&rect, lStyle, TRUE);
			::SetWindowPos(m_hRenderWnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
		}
		else
		{
			RECT rc;
			::GetWindowRect(m_hRenderWnd, &rc);
			::MoveWindow(m_hRenderWnd, rc.left, rc.top, rect.right, rect.bottom, TRUE);
		}

		ResizeBuffer();
	}
}

void CDxLibLive2d::ResizeBuffer()
{
	if (m_hRenderWnd != nullptr)
	{
		RECT rc;
		::GetClientRect(m_hRenderWnd, &rc);

		int iClientWidth = rc.right - rc.left;
		int iClientHeight = rc.bottom - rc.top;

		DxLib::SetGraphMode(iClientWidth, iClientHeight, 32);
	}
}
/*画面更新*/
void CDxLibLive2d::UpdateScreen()
{
	if (m_hRenderWnd != nullptr)
	{
		::InvalidateRect(m_hRenderWnd, nullptr, TRUE);
	}
}

void CDxLibLive2d::StartThreadpoolTimer()
{
	if (m_pTimer != nullptr)return;

	m_pTimer = ::CreateThreadpoolTimer(TimerCallback, this, nullptr);
	if (m_pTimer != nullptr)
	{
		UpdateTimerInterval(m_pTimer);
	}
}

void CDxLibLive2d::EndThreadpoolTimer()
{
	if (m_pTimer != nullptr)
	{
		::SetThreadpoolTimer(m_pTimer, nullptr, 0, 0);
		::WaitForThreadpoolTimerCallbacks(m_pTimer, TRUE);
		::CloseThreadpoolTimer(m_pTimer);
		m_pTimer = nullptr;
	}
}

void CDxLibLive2d::UpdateTimerInterval(PTP_TIMER timer)
{
	if (timer != nullptr)
	{
		FILETIME sFileDueTime{};
		ULARGE_INTEGER ulDueTime{};
		ulDueTime.QuadPart = static_cast<ULONGLONG>(-(1LL * 10 * 1000 * m_nInterval));
		sFileDueTime.dwHighDateTime = ulDueTime.HighPart;
		sFileDueTime.dwLowDateTime = ulDueTime.LowPart;
		::SetThreadpoolTimer(timer, &sFileDueTime, 0, 0);
	}
}

void CDxLibLive2d::TimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER Timer)
{
	CDxLibLive2d* pThis = static_cast<CDxLibLive2d*>(Context);
	if (pThis != nullptr)
	{
		pThis->UpdateScreen();
		pThis->UpdateTimerInterval(Timer);
	}
}
