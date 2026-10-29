#include "pch.h"
#include "MainApp.h"
#include "Cube.h"

CMainApp::CMainApp()
	: m_Cube(nullptr), m_texCube(nullptr)
{
}

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	if (!m_Cube)
		m_Cube = new CCube(Device);

	D3DXVECTOR3 vDir{1.f, -1.f, 1.f};
	D3DXCOLOR vColor = d3d::WHITE;
	D3DLIGHT9 light = d3d::InitDirectionalLight(&vDir, &vColor);
	Device->SetLight(0, &light);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 텍스처
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/crate.jpg", &m_texCube);

	// 텍스처 필터
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Delete<CCube*>(m_Cube);
	d3d::Release<IDirect3DTexture9*>(m_texCube);
}

void CMainApp::Update(float timeDelta)
{
	static float angle = -D3DX_PI * 0.5f;
	static float height = 0.f;

	if (::GetAsyncKeyState('A') & 0x8000F)
		angle -= 1.f * timeDelta;
	if (::GetAsyncKeyState('D') & 0x8000F)
		angle += 1.f * timeDelta;
	if (::GetAsyncKeyState('W') & 0x8000F)
		height += 5.f * timeDelta;
	if (::GetAsyncKeyState('S') & 0x8000F)
		height -= 5.f * timeDelta;

	// 뷰 행렬
	D3DXMATRIX view;
	D3DXVECTOR3 pos{ cosf(angle) * 3.f, height, sinf(angle) * 3.f};
	D3DXVECTOR3 target{ 0.f, 0.f, 0.f };
	D3DXVECTOR3 up{ 0.f, 1.f, 0.f };
	D3DXMatrixLookAtLH(
		&view,
		&pos,
		&target,
		&up
	);
	Device->SetTransform(D3DTS_VIEW, &view);


	static float Fov = D3DX_PI * 0.5f;

	if (::GetAsyncKeyState(VK_UP))
		Fov = clamp(Fov - 1.f * timeDelta, 0.f, 2 * D3DX_PI);
	if (::GetAsyncKeyState(VK_DOWN))
		Fov = clamp(Fov + 1.f * timeDelta, 0.f, 2 * D3DX_PI);


	// 투영 행렬
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		Fov,
		(float)g_iWinSizeX / (float)g_iWinSizeY,
		1.f,
		1000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
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

		Device->SetMaterial(&d3d::WHITE_MTRL);
		Device->SetTexture(0, m_texCube);
		m_Cube->Render(0, 0, 0);

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}