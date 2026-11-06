#include "pch.h"
#include "MainApp.h"

#include "Camera.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	HRESULT hr = 0;

	ID3DXBuffer* mtrlBuffer = {};
	DWORD numMtrls = {};

	hr = D3DXLoadMeshFromX(
		L"./Bin/Resource/mountain.x",
		D3DXMESH_MANAGED,
		Device,
		0,
		&mtrlBuffer,
		0,
		&numMtrls,
		&m_pMesh
	);

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXLoadMeshFromX() - FAILED", 0, 0);
		return false;
	}

	if (mtrlBuffer != 0 && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
		for (DWORD i = 0; i < numMtrls; ++i)
		{
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;
			m_vecMtrl.push_back(mtrls[i].MatD3D);

			if (mtrls[i].pTextureFilename != 0)
			{
				IDirect3DTexture9* tex = {};
				D3DXCreateTextureFromFileA(
					Device,
					mtrls[i].pTextureFilename,
					&tex
				);

				m_pTex.push_back(tex);
			}
			else
				m_pTex.push_back(0);

		}
	}
	d3d::Release<ID3DXBuffer*>(mtrlBuffer);

	ID3DXBuffer* errorBuffer = {};
	hr = D3DXCreateEffectFromFile(
		Device,
		L"./Bin/Resource/Effect/light_tex.fx",
		0,
		0,
		D3DXSHADER_DEBUG,
		0,
		&m_pLightTexEffect,
		&errorBuffer
	);
	if (errorBuffer)
	{
		::MessageBoxA(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	m_hWorldMatrix = m_pLightTexEffect->GetParameterByName(0, "WorldMatrix");
	m_hViewMatrix = m_pLightTexEffect->GetParameterByName(0, "ViewMatrix");
	m_hProjMatrix = m_pLightTexEffect->GetParameterByName(0, "ProjMatrix");
	m_hTex = m_pLightTexEffect->GetParameterByName(0, "Tex");

	m_hLightTexTech = m_pLightTexEffect->GetTechniqueByName("LightAndTexture");

	D3DXMATRIX W, P;
	D3DXMatrixIdentity(&W);
	m_pLightTexEffect->SetMatrix(m_hWorldMatrix, &W);
	D3DXMatrixPerspectiveFovLH(
		&P,
		D3DX_PI * 0.25f,
		(float)g_iWinSizeX / (float)g_iWinSizeY,
		1.f,
		1000.f
	);
	m_pLightTexEffect->SetMatrix(m_hProjMatrix, &P);

	IDirect3DTexture9* tex = {};
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/Terrain_3x_diffcol.jpg", &tex);
	m_pLightTexEffect->SetTexture(m_hTex, tex);


	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Release<ID3DXMesh*>(m_pMesh);
	for (int i = 0; i < m_pTex.size(); ++i)
		d3d::Release<IDirect3DTexture9*>(m_pTex[i]);
	d3d::Release<ID3DXEffect*>(m_pLightTexEffect);
}

void CMainApp::Update(float timeDelta)
{
	static float angle = (3.0f * D3DX_PI) / 2.0f;
	static float height = 5.0f;

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
		angle -= 0.5f * timeDelta;

	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
		angle += 0.5f * timeDelta;

	if (::GetAsyncKeyState(VK_UP) & 0x8000f)
		height += 5.0f * timeDelta;

	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
		height -= 5.0f * timeDelta;

	D3DXVECTOR3 position(cosf(angle) * 10.0f, height, sinf(angle) * 10.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);

	m_pLightTexEffect->SetMatrix(m_hViewMatrix, &V);

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
		// 테크 설정
		m_pLightTexEffect->SetTechnique(m_hLightTexTech);

		UINT numPasses = {};
		m_pLightTexEffect->Begin(&numPasses, 0);
		for (UINT i = 0; i < numPasses; ++i)
		{
			m_pLightTexEffect->BeginPass(i);

			for (int j = 0; j < m_vecMtrl.size(); ++j)
				m_pMesh->DrawSubset(j);

			m_pLightTexEffect->EndPass();
		}
		m_pLightTexEffect->End();

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
