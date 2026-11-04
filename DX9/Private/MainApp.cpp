#include "pch.h"
#include "MainApp.h"

#include "Camera.h"
#include "BoundingVolume.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	D3DXCreateTeapot(Device, &m_pTeapot, 0);
	m_pBSphere = CBoundingVolume::Create(CBoundingVolume::SPHERE);
	m_pBSphere->ComputeBoundingSphere(m_pTeapot);

	// SetLight
	D3DXVECTOR3 dir(0.707f, -0.f, 0.707f); 
	D3DXCOLOR col(1.f, 1.f, 1.f, 1.f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);

	// 뷰 매트릭스 설정
	D3DXVECTOR3 pos(0.f, 0.f, -10.f);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);


	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f,
		(float)g_iWinSizeX / (float)g_iWinSizeY,
		1.f,
		1000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Release<ID3DXMesh*>(m_pTeapot);
	d3d::Delete<CBoundingVolume*>(m_pBSphere);
}

void CMainApp::Update(float timeDelta)
{
	KeyInput(timeDelta);

	// update Teapot
	static float r = 0.f;
	static float v = 1.f;
	static float angle = 0.f;
	D3DXMatrixTranslation(&m_matWorld, cosf(angle) * r, sinf(angle) * r, 10.f);
	m_pBSphere->SetCenter(D3DXVECTOR3(cosf(angle) * r, sinf(angle) * r, 10.f));

	r += v * timeDelta;
	if (r >= 8.f || r <= 0.f)
		v = -v;

	angle += 1.f * D3DX_PI * timeDelta;
	if (angle >= 2 * D3DX_PI)
		angle = 0.f;
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

		// 주전자 렌더
		Device->SetTransform(D3DTS_WORLD, &m_matWorld);
		Device->SetMaterial(&d3d::YELLOW_MTRL);
		m_pTeapot->DrawSubset(0);

		// BSphere 렌더
		m_pBSphere->Render();

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}

bool CMainApp::DrawBasicScene(bool bClear, float fScale)
{
	static IDirect3DVertexBuffer9*	floor{ nullptr };
	static IDirect3DTexture9*		tex{ nullptr };
	static ID3DXMesh*				pillar{ nullptr };
	
	HRESULT hr{ 0 };

	if (bClear)
	{
		// 이미 존재하면 지운다.
		if (floor && tex && pillar)
		{
			d3d::Release<IDirect3DVertexBuffer9*>(floor);
			d3d::Release<IDirect3DTexture9*>(tex);
			d3d::Release<ID3DXMesh*>(pillar);
		}
	}
	else if (!floor && !tex && !pillar)
	{
		// 존재하지 않는 경우에는 생성해줌
		Device->CreateVertexBuffer(
			6 * sizeof(Vertex),
			0,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&floor,
			0
		);

		Vertex* v{ nullptr };
		floor->Lock(0, 0, (void**)&v, 0);
		v[0] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex(20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

		v[3] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[4] = Vertex(20.0f, -2.5f, 20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		v[5] = Vertex(20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
		floor->Unlock();

		D3DXCreateCylinder(Device, 0.5f, 0.5f, 5.f, 20, 20, &pillar, 0);
		D3DXCreateTextureFromFile(Device, L"./Bin/Resource/desert.bmp", &tex);
	}
	else
	{
		// 셋업
		Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		D3DXVECTOR3 dir(0.707f, -0.707f, 0.707f);
		D3DXCOLOR col(1.f, 1.f, 1.f, 1.f);
		D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

		Device->SetLight(0, &light);
		Device->LightEnable(0, true);
		Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		Device->SetRenderState(D3DRS_SPECULARENABLE, true);

		D3DXMATRIX T, R, P, S;
		
		D3DXMatrixScaling(&S, fScale, fScale, fScale);

		D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

		// 바닥 그리기
		D3DXMatrixIdentity(&T);
		T = T * S;
		Device->SetTransform(D3DTS_WORLD, &T);
		Device->SetMaterial(&d3d::WHITE_MTRL);
		Device->SetTexture(0, tex);
		Device->SetStreamSource(0, floor, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// 기둥 그리기
		Device->SetMaterial(&d3d::BLUE_MTRL);
		Device->SetTexture(0, 0); 
		for (int i = 0; i < 5; ++i)
		{
			D3DXMatrixTranslation(&T, -5.f, 0.f, -15.f + (i * 7.5f));
			P = R * T * S;
			Device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);

			D3DXMatrixTranslation(&T, 5.f, 0.f, -15.f + (i * 7.5f));
			P = R * T * S;
			Device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);
		}
	}
	return true;
}

void CMainApp::KeyInput(float timeDelta)
{
	POINT lp;
	GetCursorPos(&lp);
	ScreenToClient(g_hWnd, &lp);

	Ray ray = CalcPickingRay(lp.x, lp.y);
	D3DXMATRIX view;
	Device->GetTransform(D3DTS_VIEW, &view);
	D3DXMATRIX viewInverse;
	D3DXMatrixInverse(&viewInverse, 0, &view);

	TransformRay(&ray, &viewInverse);

	// 힛 테스트
	if (::GetAsyncKeyState(VK_LBUTTON) && RaySphereIntTest(&ray, m_pBSphere))
		::MessageBox(0, L"HIT!", L"HIT!", 0); 
}

Ray CMainApp::CalcPickingRay(int x, int y) // s_x, s_y
{
	// 투영창 좌표
	float px = 0.f;
	float py = 0.f;

	D3DVIEWPORT9 vp;
	Device->GetViewport(&vp);

	D3DXMATRIX proj;
	Device->GetTransform(D3DTS_PROJECTION, &proj);

	// s_x, s_y를 구할 때, 뷰포트 변환을 진행하는데, s_x, s_y와 뷰포트 변환 행렬은 아니깐 역으로 뷰 스페이스 p_x, p_y를 구함
	// proj으로 나눠주는 이유는, NDC가 될 때 배율이 적용되기 때문에 그걸 돌려주는 것임
	px = (((2.f * x) / vp.Width) - 1.f) / proj(0, 0);
	py = (((-2.f * y) / vp.Height) + 1.f) / proj(1, 1);

	Ray ray;
	ray._origin = D3DXVECTOR3(0.f, 0.f, 0.f);
	ray._direction = D3DXVECTOR3(px, py, 1.f);
	return ray;
}

void CMainApp::TransformRay(Ray* ray, D3DXMATRIX* T)
{
	// 뷰 스페이스를 월드 스페이스로 변환하기 위한 함수

	D3DXVec3TransformCoord(
		&ray->_origin,
		&ray->_origin,
		T
	);
	D3DXVec3TransformNormal(
		&ray->_direction,
		&ray->_direction,
		T
	);
	D3DXVec3Normalize(&ray->_direction, &ray->_direction);
}

// 실제 Ray가 경계 볼륨에 충돌한지
// - 즉, 클릭이 물체와 충돌했는지 확인
bool CMainApp::RaySphereIntTest(Ray* ray, CBoundingVolume* sphere)
{
	D3DXVECTOR3 v = ray->_origin - sphere->GetCenter();

	float b = 2.f * D3DXVec3Dot(&ray->_direction, &v);
	float c = D3DXVec3Dot(&v, &v) - (sphere->GetRadius() * sphere->GetRadius());

	float discriminant = (b * b) - (4.f * c);
	if (discriminant < 0.f)
		return false;

	float s0 = (-b + discriminant) / 2.f;
	float s1 = (-b - discriminant) / 2.f;

	if (s0 >= 0.f || s1 >= 0.f)
		return true;

	return false;
}
