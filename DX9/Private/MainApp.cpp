#include "pch.h"
#include "MainApp.h"

#include "Cube.h"
#include "Quad.h"

CMainApp::CMainApp()
	: m_pCube(nullptr), m_pTexCube(nullptr), m_pQuad(nullptr), m_pTexQuad(nullptr)
{
}

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	// 큐브, 배경 사각 생성
	m_pCube = CCube::Create(Device);
	m_pQuad = CQuad::Create(Device);

	// 텍스처 불러오기
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/lobbyxpos.jpg", &m_pTexQuad);
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/cratewalpha.dds", &m_pTexCube);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 알파 블렌딩 세팅
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	Device->SetRenderState(D3DRS_LIGHTING, false);

	// 월드 행렬 생성
	D3DXMatrixIdentity(&m_matWorldCube);


	// 뷰 행렬 생성
	D3DXVECTOR3 pos(0.f, 0.f, -2.5f);
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
	d3d::Delete<CCube*>(m_pCube);
	d3d::Delete<CQuad*>(m_pQuad);
	d3d::Release<IDirect3DTexture9*>(m_pTexCube);
	d3d::Release<IDirect3DTexture9*>(m_pTexQuad);
}

void CMainApp::Update(float timeDelta)
{
	D3DXMATRIX xRot;
	D3DXMatrixRotationX(&xRot, D3DX_PI * 0.2f);

	static float y = 0.f;
	D3DXMATRIX yRot;
	D3DXMatrixRotationY(&yRot, y);
	y += timeDelta;
	if (y >= D3DX_PI * 2.f)
		y = 0.f; 

	m_matWorldCube = xRot * yRot;
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

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		m_pQuad->Render(&I, 0, m_pTexQuad);
		m_pCube->Render(&m_matWorldCube, 0, m_pTexCube);

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}