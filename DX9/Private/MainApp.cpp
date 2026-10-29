#include "pch.h"
#include "MainApp.h"
#include "Quad.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	Device->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &m_VB, 0);
	CQuad Quad; int iIdx(0);
	Vertex* vertices;
	m_VB->Lock(0, 0, (void**)&vertices, 0);
	for (const Vertex& vtx : Quad.GetVtxList())
		vertices[iIdx++] = vtx;
	m_VB->Unlock();

	// 텍스처 활성화
	Device->SetTexture(0, Quad.GetTex());

	// 텍스처 필터 지정
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT); // 가장 가까운 밉맵 사용

	// 투영 행렬 지정
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)g_iWinSizeX / (float)g_iWinSizeY,
		1.f,
		1000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	// 조명 이용 X
	Device->SetRenderState(D3DRS_LIGHTING, false); 

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
		Device->BeginScene();

		Device->SetStreamSource(0, m_VB, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);

		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}