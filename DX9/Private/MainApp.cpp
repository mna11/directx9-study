#include "pch.h"
#include "MainApp.h"

#include "Teapot.h"
#include "Quad.h"

CMainApp::CMainApp()
	: m_pTeapot(nullptr), m_pQuad(nullptr), m_pTexQuad(nullptr)
{
	::ZeroMemory(&m_mtrlQuad, sizeof(m_mtrlQuad));
	::ZeroMemory(&m_mtrlTeapot, sizeof(m_mtrlTeapot));
}

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	// 마테리얼 초기화
	m_mtrlTeapot = d3d::RED_MTRL;
	m_mtrlTeapot.Diffuse.a = 0.5f; // Diffuse의 반사가 물체의 기본색

	m_mtrlQuad = d3d::WHITE_MTRL;

	// 주전자, 배경 사각 생성
	m_pTeapot = CTeapot::Create(Device);
	m_pQuad = CQuad::Create(Device);

	// 빛 초기화 
	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = d3d::WHITE;
	dir.Specular = d3d::WHITE * 0.2f;
	dir.Ambient = d3d::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3{ 0.707f, 0.f, 0.707f };

	Device->SetLight(0, &dir);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 텍스처 불러오기
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/crate.jpg", &m_pTexQuad);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 알파 블렌딩 세팅
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 월드 행렬 생성
	D3DXMatrixIdentity(&m_matWorldQuad);
	D3DXMatrixIdentity(&m_matWorldTeapot);
	D3DXMatrixScaling(&m_matWorldTeapot, 1.5f, 1.5f, 1.5f);


	// 뷰 행렬 생성
	D3DXVECTOR3 pos(0.f, 0.f, -3.f);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &view);

	// 투영 행렬 생성
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)g_iWinSizeX / (float)g_iWinSizeY,
		1.f,
		1000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Delete<CTeapot*>(m_pTeapot);
	d3d::Delete<CQuad*>(m_pQuad);
	d3d::Release<IDirect3DTexture9*>(m_pTexQuad);
}

void CMainApp::Update(float timeDelta)
{
	if (::GetAsyncKeyState('A') & 0x8000f)
		m_mtrlTeapot.Diffuse.a = clamp(m_mtrlTeapot.Diffuse.a + 0.1f * timeDelta, 0.f, 1.f);
	if (::GetAsyncKeyState('S') & 0x8000f)
		m_mtrlTeapot.Diffuse.a = clamp(m_mtrlTeapot.Diffuse.a - 0.1f * timeDelta, 0.f, 1.f);
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
		Device->BeginScene();

		m_pQuad->Render(&m_matWorldQuad, &m_mtrlQuad, m_pTexQuad);
		m_pTeapot->Render(&m_matWorldTeapot, &m_mtrlTeapot, 0);

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}