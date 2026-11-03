#include "pch.h"
#include "MainApp.h"

#include "Camera.h"
#include "Snow.h"
#include "Firework.h"
#include "ParticleGun.h"
#include "BoundingVolume.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	srand((unsigned int)time(0));

	m_pCamera = new CCamera;

	switch (m_eParticleType)
	{
	case SNOW:
	{
		CBoundingVolume* pBBox = CBoundingVolume::Create(CBoundingVolume::BOX);
		pBBox->SetMin({ -10.f, -10.f, -10.f });
		pBBox->SetMax({ 10.f, 10.f, 10.f });
		m_pParticle = new psys::CSnow(pBBox, 5000);
		m_pParticle->Init(L"./Bin/Resource/snowflake.dds");
		break;
	}
	case FIREWORK:
	{
		D3DXVECTOR3 origin(0.f, 10.f, 50.f);
		m_pParticle = new psys::CFirework(&origin, 6000);
		m_pParticle->Init(L"./Bin/Resource/flare.bmp");
		break;
	}
	case GUN:
	{
		m_pParticle = new psys::CParticleGun(m_pCamera);
		m_pParticle->Init(L"./Bin/Resource/flare_alpha.dds");
	}
	default:
		break;
	}


	DrawBasicScene(false, 0.f); // 기본 씬 초기화

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f,
		(float)g_iWinSizeX / (float)g_iWinSizeY,
		1.f,
		5000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	DrawBasicScene(true, 0.f);
	d3d::Delete<CCamera*>(m_pCamera);
	d3d::Delete<psys::CPSystem*>(m_pParticle);
}

void CMainApp::Update(float timeDelta)
{
	KeyInput(timeDelta);

	// 뷰 행렬
	D3DXMATRIX V;
	m_pCamera->GetViewMatrix(&V);
	Device->SetTransform(D3DTS_VIEW, &V);

	m_pParticle->Update(timeDelta);

	if (m_eParticleType == FIREWORK)
	{
		if (m_pParticle->IsDead())
			m_pParticle->reset();
	}
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
		Device->SetTransform(D3DTS_WORLD, &I);

		DrawBasicScene(false, 1.f);

		Device->SetTransform(D3DTS_WORLD, &I);
		m_pParticle->Render();

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
	// 앞 뒤 이동
	if (::GetAsyncKeyState('W') & 0x8000)
		m_pCamera->Walk(4.f * timeDelta);
	if (::GetAsyncKeyState('S') & 0x8000)
		m_pCamera->Walk(-4.f * timeDelta);
	// 좌 우 이동
	if (::GetAsyncKeyState('A') & 0x8000)
		m_pCamera->Strafe(-4.f * timeDelta);
	if (::GetAsyncKeyState('D') & 0x8000)
		m_pCamera->Strafe(4.f * timeDelta);
	// 상 하 이동
	if (::GetAsyncKeyState('R') & 0x8000)
		m_pCamera->Fly(4.f * timeDelta);
	if (::GetAsyncKeyState('F') & 0x8000)
		m_pCamera->Fly(-4.f * timeDelta);
	// Pitch
	if (::GetAsyncKeyState(VK_UP) & 0x8000)
		m_pCamera->Pitch(1.f * timeDelta);
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_pCamera->Pitch(-1.f * timeDelta);
	// Yaw
	if (::GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_pCamera->Yaw(-1.f * timeDelta);
	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_pCamera->Yaw(1.f * timeDelta);
	// Roll
	if (::GetAsyncKeyState('N') & 0x8000)
		m_pCamera->Roll(1.f * timeDelta);
	if (::GetAsyncKeyState('M') & 0x8000)
		m_pCamera->Roll(-1.f * timeDelta);

	if (m_eParticleType == GUN)
	{
		if (::GetAsyncKeyState(VK_SPACE) & 0x8000)
			m_pParticle->AddParticle();
	}
}
