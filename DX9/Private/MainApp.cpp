#include "pch.h"
#include "MainApp.h"
#include "Defines.h"
#include "Triangle.h"

CMainApp::CMainApp()
	: m_VB(nullptr)
{
}

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	Device->CreateVertexBuffer(3 * sizeof(ColorVertex), D3DUSAGE_WRITEONLY, ColorVertex::FVF, D3DPOOL_MANAGED, &m_VB, 0);

	CTriangle tri;
	int iIdx(0);

	ColorVertex* vertices;
	m_VB->Lock(0, 0, (void**)&vertices, 0);
	for (ColorVertex cvtx : tri.GetVtxList())
		vertices[iIdx++] = cvtx;
	m_VB->Unlock();

	// 투영 매트릭스 지정
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		static_cast<float>(g_iWinSizeX) / static_cast<float>(g_iWinSizeY),
		1.f,
		1000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	// 렌더 상태 지정
	Device->SetRenderState(D3DRS_LIGHTING, false);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_VB);
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
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.f, 0);

		Device->BeginScene();

		Device->SetFVF(ColorVertex::FVF);
		Device->SetStreamSource(0, m_VB, 0, sizeof(ColorVertex));

		// 플랫 쉐이딩으로 화면 왼쪽에 삼각형을 그린다.
		D3DXMATRIX world;
		D3DXMatrixTranslation(&world, -1.25f, 0.f, 0.f);
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		// 인덱스 버퍼는 선택 사항으로, 이렇게 정점 버퍼에 저장된 순서대로 그릴 수도 있다.
		// 인덱싱은 여러 삼각형이 정점을 공유할 떄, 유리
		// 지금은 단순 삼각형 하나 출력이라 이렇게 함
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		// 고러드 쉐이딩으로 화면 오른쪽에 삼각형을 그린다.
		D3DXMatrixTranslation(&world, 1.25f, 0.f, 0.f);
		Device->SetTransform(D3DTS_WORLD, &world);
		Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		Device->EndScene();

		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}