#include "pch.h"
#include "MainApp.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
}

void CMainApp::Update(float timeDelta)
{
}

bool CMainApp::Display() // Render의 역할
{
	if (Device)
	{
		// 초기화
		// 전체 화면을 지우겠다 (0, 0)
		// Target Surface와 깊이 버퍼를 지우겠다.
		// 검정색, z 값은 1로 (가장 멀게), 스텐실은 0으로
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0000000, 1.f, 0);

		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}