#include "pch.h"
#include "MainApp.h"
#include "Defines.h"

#include "Cube.h"
#include "Teapot.h"

CMainApp::CMainApp()
	: m_VB(nullptr), m_IB(nullptr),
	m_pCube(nullptr), m_pTeapot(nullptr), m_pTriangle(nullptr),
	m_eRenderObj(TEAPOT)
{
}

CMainApp::~CMainApp()
{
}

bool CMainApp::Setup() // Initialize의 역할
{
	// 버텍스 버퍼 만들기
	Device->CreateVertexBuffer(8 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &m_VB, 0);
	// 인덱스 버퍼 만들기
	Device->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_IB, 0);

	switch (m_eRenderObj)
	{
	case CUBE:
	{
		m_pCube = new CCube();

		// 큐브 데이터로 버퍼를 채운다.
		Vertex* vertices;

		// Lock은 CPU가 버퍼 메모리에 접근할 수 있도록 준비하고, 그 메모리의 주소를 vertices에 넣어준다.
		// 0, 0은 버퍼 전체에 접근하겠다.
		m_VB->Lock(0, 0, (void**)&vertices, 0);

		const list<Vertex>& vtxList = m_pCube->GetVtxList();
		int iIdx(0);
		for (Vertex vtx : vtxList)
			vertices[iIdx++] = vtx;

		m_VB->Unlock();

		// 큐브의 삼각형을 정의한다.
		WORD* indices = 0;
		m_IB->Lock(0, 0, (void**)&indices, 0);

		const list<WORD>& idxList = m_pCube->GetIdxList();
		iIdx = 0;
		for (WORD idx : idxList)
			indices[iIdx++] = idx;

		m_IB->Unlock();

		break;
	}
	case TEAPOT:
	{
		m_pTeapot = new CTeapot();
		break;
	}
	}
	
	// 카메라의 위치와 방향을 조정한다.
	D3DXVECTOR3 position(0.f, 0.f, -5.f);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);

	Device->SetTransform(D3DTS_VIEW, &V);

	// 투영 행렬을 지정한다.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f,
		static_cast<float>(g_iWinSizeX) / static_cast<float>(g_iWinSizeY),
		1.f, 1000.f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	// 렌더 상태를 지정한다.
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	
	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_VB);
	d3d::Release<IDirect3DIndexBuffer9*>(m_IB);

	d3d::Delete<CCube*>(m_pCube);
	d3d::Delete<CTeapot*>(m_pTeapot);
}

void CMainApp::Update(float timeDelta)
{
	if (!Device)
		return;

	// 큐브 회전
	D3DXMATRIX Rx, Ry;
	// X축으로 45도 회전
	D3DXMatrixRotationX(&Rx, D3DX_PI * 0.25f);

	static float y(0.f);
	D3DXMatrixRotationY(&Ry, y);
	y += timeDelta;
	if (y >= 2 * D3DX_PI)
		y = 0.f;

	D3DXMATRIX p = Rx * Ry;
	Device->SetTransform(D3DTS_WORLD, &p);
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

		switch (m_eRenderObj)
		{
		case CUBE:
		case TRIANGLE:
			Device->SetStreamSource(0, m_VB, 0, sizeof(Vertex));
			Device->SetIndices(m_IB);
			Device->SetFVF(Vertex::FVF);
			Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
			break;
		case TEAPOT:
			m_pTeapot->GetMesh()->DrawSubset(0);
			break;
		}
		
		Device->EndScene();

		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}