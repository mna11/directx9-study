#include "pch.h"
#include "MainApp.h"
#include "Pyramid.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	// 조명 연산을 하겠다고 선언하는 것, 그러나 DX9는 디폴트로 활성되어 있으나 그냥 명시한 것
	Device->SetRenderState(D3DRS_LIGHTING, true);
	// 버텍스 버퍼 초기화
	Device->CreateVertexBuffer(
		12 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&m_VB,
		0
	);


	CPyramid Pyramid;
	int iIdx(0);

	Vertex* vertices;
	m_VB->Lock(0, 0, (void**)&vertices, 0);

	for (const Vertex& vtx : Pyramid.GetVtxList())
		vertices[iIdx++] = vtx;

	m_VB->Unlock();

	// Material 정의
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = d3d::WHITE;
	mtrl.Diffuse = d3d::WHITE;
	mtrl.Specular = d3d::WHITE;
	mtrl.Emissive = d3d::BLACK;
	mtrl.Power = 5.f;

	Device->SetMaterial(&mtrl);

	// 방향성 광원 만들기
	// - x축과 평행하게 빛 발산 (방향성 광원)
	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = d3d::WHITE;
	dir.Specular = d3d::WHITE * 0.3f;
	dir.Ambient = d3d::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(1.f, 0.f, 0.f);

	// 광원 설정
	Device->SetLight(0, &dir);
	Device->LightEnable(0, true);

	// 법선 정리, 정반사광 활성화 
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 뷰 행렬 설정
	D3DXVECTOR3 pos(0.f, 1.f, -3.f);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V); 

	// 투영 행렬 설정
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



	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_VB);
}

void CMainApp::Update(float timeDelta)
{
	D3DXMATRIX yRot;
	static float y = 0.f;
	D3DXMatrixRotationY(&yRot, y);
	y += timeDelta;
	if (y >= 2 * D3DX_PI)
		y = 0.f;

	Device->SetTransform(D3DTS_WORLD, &yRot);
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
		
		Device->SetStreamSource(0, m_VB, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}