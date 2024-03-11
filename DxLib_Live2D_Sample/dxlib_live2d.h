#ifndef DXLIB_LIVE2D_H_
#define DXLIB_LIVE2D_H_

#include <Windows.h>

#include <string>
#include <vector>

#define DX_NON_USING_NAMESPACE_DXLIB
#include <DxLib.h>

namespace dxlib_live2d
{
	struct SMotion
	{
		std::wstring wstrMotionGroupName;
		int iLargestMotionId = 0;
	};
}

class CDxLibLive2d
{
public:
	CDxLibLive2d();
	~CDxLibLive2d();
	bool Setup(HWND hRenderWnd, const std::wstring &wstrDllPath);
	bool LoadModel(const std::wstring& wstrModelPath);
	void Display();
	void SwitchMotion();
	bool SwitchPause();

	void RescaleTime(bool bHasten);
	void RescaleModel(bool bUpscale);
	void RotateModel(bool bClockWise);
	void ResetScale();
	void SwitchSizeLore(bool bBarHidden);
	void SetOffset(int iX, int iY);
private:
	HWND m_hRenderWnd = nullptr;
	int m_iDxLibInitialised = -1;

	enum Constants { kInterval = 32, kBaseWidth = 1280, kBaseHeight = 720};
	const float m_fScalePortion = 0.05f;

	int m_iModelHandle = -1;

	float m_fCanvasWidth = Constants::kBaseWidth;
	float m_fCanvasHeight = Constants::kBaseHeight;

	std::vector<std::wstring> m_expressionNames;
	size_t m_nExpressionIndex = 0;
	std::vector<dxlib_live2d::SMotion> m_motions;
	size_t m_nMotionGroupIndex = 0;
	int m_iMotionIndex = 0;
	std::vector<std::wstring> m_hitAreaNames;

	long long m_nInterval = Constants::kInterval;
	int m_iXOffset = 0;
	int m_iYOffset = 0;
	float m_fScale = 1.f;
	float m_fRotation = 0.f;

	bool m_bBarHidden = false;
	bool m_bPaused = false;

	void ClearModelInfo();
	void GetExpressionNames();
	void GetMotionNames();
	void GetHitAreaNames();
	void GetCamvasSize();

	void IncrementMotionIndex();

	void DisposeModel();
	void UpdateModelScale();
	void UpdateModelOrigin();
	void UpdateModelRotation();

	void ResizeWindow();
	void ResizeBuffer();
	void UpdateScreen();

	PTP_TIMER m_pTimer = nullptr;

	void StartThreadpoolTimer();
	void EndThreadpoolTimer();
	void UpdateTimerInterval(PTP_TIMER timer);
	static void CALLBACK TimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER Timer);
};

#endif // !DXLIB_LIVE2D_H_
