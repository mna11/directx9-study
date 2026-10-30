#include "pch.h"
#include "MainApp.h"

CMainApp::CMainApp()
	: m_Font(0), m_FrameCnt(0), m_TimeElapsed(0.f), m_FPS(0), m_strFPS({})
{
}

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	// 책은 LOGFONT인데 지금은 D3DXFONT_DESCW인듯
	D3DXFONT_DESCW lf{};
	lf.Height = 25;                   // 글자 셀의 높이
	lf.Width = 12;                    // 글자의 너비
	lf.Weight = 500;                  // 굵기: FW_MEDIUM
	lf.MipLevels = 1;                 // 글꼴 텍스처의 밉 레벨 수
	lf.Italic = false;                // 기울임체 사용 여부
	lf.CharSet = DEFAULT_CHARSET;     // 문자 집합
	lf.OutputPrecision = 0;           // 글꼴 선택 방식: 기본값
	lf.Quality = 0;                   // 출력 품질: 기본값
	lf.PitchAndFamily = 0;            // 폭 방식과 글꼴 계열: 기본값
	lstrcpyW(lf.FaceName, L"Times New Roman"); // 글꼴 이름

	// ID3DXFont 생성
	if (FAILED(D3DXCreateFontIndirect(Device, &lf, &m_Font)))
	{
		::MessageBox(0, L"D3DXCreateFontIndirect() - FAILED", 0, 0);
		::PostQuitMessage(0);
	}

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Release<ID3DXFont*>(m_Font);
}

void CMainApp::Update(float timeDelta)
{
	// FPS
	++m_FrameCnt;
	m_TimeElapsed += timeDelta;
	if (m_TimeElapsed >= 1.f)
	{
		m_FPS = (float)m_FrameCnt / m_TimeElapsed;
		m_strFPS = to_wstring(m_FPS);

		m_TimeElapsed = 0.f;
		m_FrameCnt = 0;
	}
}

bool CMainApp::Display() // Render의 역할
{
	if (Device)
	{
		// 초기화
		// 전체 화면을 지우겠다 (0, 0)
		// Target Surface와 깊이 버퍼를 지우겠다.
		// 검정색, z 값은 1로 (가장 멀게), 스텐실은 0으로
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.f, 0);
		Device->BeginScene();

		RECT rt = { 0, 0, g_iWinSizeX, g_iWinSizeY };
		m_Font->DrawText(
			NULL,				// 책에는 없었는데, 텍스처를 이용한 2D 그리기를 도와주는 객체 (ID3DXSprite)
			m_strFPS.c_str(),
			-1,
			&rt,
			DT_TOP | DT_LEFT,
			0xff000000
		);

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}