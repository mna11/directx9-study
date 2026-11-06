#include "pch.h"
#include "MainApp.h"

#include "Camera.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	HRESULT hr = { 0 };
	ID3DXBuffer* adjBuffer[4] = { nullptr, nullptr, nullptr, nullptr };

	D3DXCreateTeapot(Device, &m_pMeshes[0], &adjBuffer[0]);
	D3DXCreateSphere(Device, 1.f, 20, 20, &m_pMeshes[1], &adjBuffer[1]);
	D3DXCreateTorus(Device, 0.5f, 1.f, 20, 20, &m_pMeshes[2], &adjBuffer[2]);
	D3DXCreateCylinder(Device, 0.5f, 0.5f, 2.f, 20, 20, &m_pMeshes[3], &adjBuffer[3]);

	D3DXMatrixTranslation(&m_matWorld[0], 0.f, 2.f, 0.f);
	D3DXMatrixTranslation(&m_matWorld[1], 0.f, -2.f, 0.f);
	D3DXMatrixTranslation(&m_matWorld[2], -3.f, 0.f, 0.f);
	D3DXMatrixTranslation(&m_matWorld[3], 3.f, 0.f, 0.f);

	m_vMeshColor[0] = D3DXVECTOR4(1.f, 0.f, 0.f, 1.f);
	m_vMeshColor[1] = D3DXVECTOR4(0.f, 1.f, 0.f, 1.f);
	m_vMeshColor[2] = D3DXVECTOR4(0.f, 0.f, 1.f, 1.f);
	m_vMeshColor[3] = D3DXVECTOR4(1.f, 1.f, 0.f, 1.f);

	// 이펙트
	ID3DXBuffer* errorBuffer = {};
	hr = D3DXCreateEffectFromFile(
		Device,
		L"./Bin/Resource/Effect/toon.fx",
		0, 0,
		D3DXSHADER_DEBUG,
		0,
		&m_pToonEffect,
		&errorBuffer
	);


	if (errorBuffer)
	{
		::MessageBoxA(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	// 텍스처 로드
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/toonshade.bmp", &m_pShadeTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	// 핸들
	m_hToonTech = m_pToonEffect->GetTechniqueByName("Toon");
	m_hShadeTex = m_pToonEffect->GetParameterByName(0, "ShadeTex");
	m_hToonWorldView = m_pToonEffect->GetParameterByName(0, "WorldViewMatrix");
	m_hToonWorldViewProj = m_pToonEffect->GetParameterByName(0, "WorldViewProjMatrix");
	m_hToonColor = m_pToonEffect->GetParameterByName(0, "Color");
	m_hToonLightDir = m_pToonEffect->GetParameterByName(0, "LightDirection");

	D3DXVECTOR4 directionToLight(-0.57f, 0.57f, -0.57f, 0.f);
	m_pToonEffect->SetVector(
		m_hToonLightDir,
		&directionToLight
	);

	// 투영 행렬
	D3DXMatrixPerspectiveFovLH(
		&m_matProj, D3DX_PI * 0.25f,
		(float)g_iWinSizeX / (float)g_iWinSizeY, 1.f, 1000.f
	);

	// 텍스처
	IDirect3DTexture9* tex = {};
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/toonshade.bmp", &tex);
	m_pToonEffect->SetTexture(m_hShadeTex, tex);
	d3d::Release<IDirect3DTexture9*>(tex);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	for (int i = 0; i < 4; i++)
	{
		d3d::Release<ID3DXMesh*>(m_pMeshes[i]);
	}
	d3d::Release<IDirect3DTexture9*>(m_pShadeTex);
	d3d::Release<ID3DXEffect*>(m_pToonEffect);
}

void CMainApp::Update(float timeDelta)
{
	static float angle = (3.f * D3DX_PI) / 2.f;
	static float height = 5.f;

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000)
		angle -= 0.5f * timeDelta;
	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000)
		angle += 0.5f * timeDelta;
	if (::GetAsyncKeyState(VK_UP) & 0x8000)
		height += 5.f * timeDelta;
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000)
		height -= 5.f * (timeDelta);

	// 뷰 매트릭스
	D3DXVECTOR3 position(cosf(angle) * 10.f, height, sinf(angle) * 10.f);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXMatrixLookAtLH(&m_matView, &position, &target, &up);
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
		
		m_pToonEffect->SetTechnique(m_hToonTech);

		UINT numPasses = {};
		m_pToonEffect->Begin(&numPasses, 0);

		D3DXMATRIX WorldView;
		D3DXMATRIX WorldViewProj;
		for (int i = 0; i < numPasses; ++i)
		{
			m_pToonEffect->BeginPass(i);

			for (int j = 0; j < 4; ++j)
			{
				WorldView = m_matWorld[j] * m_matView;
				WorldViewProj = m_matWorld[j] * m_matView * m_matProj;

				m_pToonEffect->SetMatrix(m_hToonWorldView, &WorldView);
				m_pToonEffect->SetMatrix(m_hToonWorldViewProj, &WorldViewProj);
				m_pToonEffect->SetVector(m_hToonColor, &m_vMeshColor[j]);
				m_pToonEffect->CommitChanges();
				m_pMeshes[j]->DrawSubset(0);
			}

		}
		m_pToonEffect->EndPass();
		m_pToonEffect->End();

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}

bool CMainApp::DrawBasicScene(bool bClear, float fScale)
{
	static IDirect3DVertexBuffer9* floor{ nullptr };
	static IDirect3DTexture9* tex{ nullptr };
	static ID3DXMesh* pillar{ nullptr };

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