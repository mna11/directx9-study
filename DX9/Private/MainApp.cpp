#include "pch.h"
#include "MainApp.h"
#include "d3dfont.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	// Font 생성
	m_Font = new CD3DFont(L"Times New Roman", 16, 0);
	m_Font->InitDeviceObjects(Device);
	m_Font->RestoreDeviceObjects();

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	if (m_Font)
	{
		m_Font->InvalidateDeviceObjects();
		m_Font->DeleteDeviceObjects();
		delete m_Font;
		m_Font = nullptr;
	}
}

void CMainApp::Update(float timeDelta)
{
	++m_FrameCnt;

	m_TimeElapsed += timeDelta;

	if (m_TimeElapsed >= 1.0f)
	{
		m_FPS = (float)m_FrameCnt / m_TimeElapsed;

		m_strFPS = to_wstring(m_FPS);
		
		m_TimeElapsed = 0.0f;
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

		if (m_Font)
			m_Font->DrawText(20, 20, 0xff000000, m_strFPS.c_str());

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}