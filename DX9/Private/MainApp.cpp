#include "pch.h"
#include "MainApp.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	m_MeshObjList.resize(4, nullptr);
	m_matWorldList.resize(4);

	// 오브젝트 생성
	D3DXCreateTeapot(Device, &m_MeshObjList[0], 0);
	D3DXCreateSphere(Device, 1.f, 20, 20, &m_MeshObjList[1], 0);
	D3DXCreateTorus(Device, 0.5f, 1.f, 20, 20, &m_MeshObjList[2], 0);
	D3DXCreateCylinder(Device, 0.5f, 0.5f, 2.f, 20, 20, &m_MeshObjList[3], 0);

	// 월드 행렬
	D3DXMatrixTranslation(&m_matWorldList[0], 0.f, 2.f, 0.f);
	D3DXMatrixTranslation(&m_matWorldList[1], 0.f, -2.f, 0.f);
	D3DXMatrixTranslation(&m_matWorldList[2], -3.f, 0.f, 0.f);
	D3DXMatrixTranslation(&m_matWorldList[3], 3.f, 0.f, 0.f);

	// Material 설정
	m_mtrlList.emplace_back(d3d::RED_MTRL);
	m_mtrlList.emplace_back(d3d::BLUE_MTRL);
	m_mtrlList.emplace_back(d3d::GREEN_MTRL);
	m_mtrlList.emplace_back(d3d::YELLOW_MTRL);

	// 방향성 광원 설정
	D3DXVECTOR3 dir(1.f, 0.f, 0.25f);
	D3DXCOLOR c = d3d::WHITE;
	D3DLIGHT9 dirLight = d3d::InitDirectionalLight(&dir, &c);

	// light 설정
	Device->SetLight(0, &dirLight);
	Device->LightEnable(0, true);

	// light - render state 설정
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, false);

	// 조명 연산을 하겠다고 선언하는 것, 그러나 DX9는 디폴트로 활성되어 있으나 그냥 명시한 것
	Device->SetRenderState(D3DRS_LIGHTING, true);

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
}

void CMainApp::Cleanup() // Release의 역할
{
}

void CMainApp::Update(float timeDelta)
{
	// 카메라 포지션
	static float angle = (3.f * D3DX_PI) / 2.f; // 270도
	static float height = 5.f;

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000F)
		angle -= 0.5f * timeDelta;
	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000F)
		angle += 0.5f * timeDelta;
	if (::GetAsyncKeyState(VK_UP) & 0x8000F)
		height += 0.5f * timeDelta;
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000F)
		height -= 0.5f * timeDelta;

	// (xz평면)반지름이 7인 원의 원주를 따라서 회전할거다. 
	D3DXVECTOR3 position(cosf(angle) * 7.f, height, sinf(angle) * 7.f);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

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
		
		for (int i = 0; i < m_MeshObjList.size(); ++i)
		{
			Device->SetMaterial(&m_mtrlList[i]);
			Device->SetTransform(D3DTS_WORLD, &m_matWorldList[i]);
			m_MeshObjList[i]->DrawSubset(0);
		}
		

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}