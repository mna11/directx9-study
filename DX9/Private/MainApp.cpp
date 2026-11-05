#include "pch.h"
#include "MainApp.h"

#include "Camera.h"
#include "SilhouetteEdge.h"

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

	for (int i = 0; i < 4; ++i)
		m_pMeshOutline[i] = new CSilhouetteEdge(m_pMeshes[i], adjBuffer[i]);

	d3d::Release<ID3DXBuffer*>(adjBuffer[0]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[1]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[2]);
	d3d::Release<ID3DXBuffer*>(adjBuffer[3]);

	// 셰이더 컴파일
	ID3DXBuffer* toonCompiledCode = { nullptr };
	ID3DXBuffer* toonErrorBuffer = { nullptr };

	hr = D3DXCompileShaderFromFile(
		L"./Bin/Resource/Shader/toon.hlsl",
		0,
		0,
		"main",
		"vs_1_1",
		D3DXSHADER_DEBUG,
		&toonCompiledCode,
		&toonErrorBuffer,
		&m_pToonConstTable
	);

	if (toonErrorBuffer)
	{
		::MessageBoxA(0, (char*)toonErrorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(toonErrorBuffer);
	}

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = Device->CreateVertexShader(
		(DWORD*)toonCompiledCode->GetBufferPointer(),
		&m_pToonShader
	);

	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexShader() - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(toonCompiledCode);

	// Outline Shader

	ID3DXBuffer* outlineCompiledCode = { nullptr };
	ID3DXBuffer* outlineErrorBuffer = { nullptr };

	hr = D3DXCompileShaderFromFile(
		L"./Bin/Resource/Shader/outline.hlsl",
		0,
		0,
		"main",
		"vs_1_1",
		D3DXSHADER_DEBUG,
		&outlineCompiledCode,
		&outlineErrorBuffer,
		&m_pOutlineConstTable
	);

	if (outlineErrorBuffer)
	{
		::MessageBoxA(0, (char*)outlineErrorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(outlineErrorBuffer);
	}

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	hr = Device->CreateVertexShader(
		(DWORD*)outlineCompiledCode->GetBufferPointer(),
		&m_pOutlineShader);

	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(outlineCompiledCode);


	// 텍스처 로드
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/toonshade.bmp", &m_pShadeTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	// 핸들

	m_hToonWorldView = m_pToonConstTable->GetConstantByName(0, "WorldViewMatrix");
	m_hToonWorldViewProj = m_pToonConstTable->GetConstantByName(0, "WorldViewProjMatrix");
	m_hToonColor = m_pToonConstTable->GetConstantByName(0, "Color");
	m_hToonLightDir = m_pToonConstTable->GetConstantByName(0, "LightDirection");

	m_hOutlineWorldView = m_pOutlineConstTable->GetConstantByName(0, "WorldViewMatrix");
	m_hOutlineProjHandle = m_pOutlineConstTable->GetConstantByName(0, "ProjMatrix");

	// Shader Constants 세팅
	D3DXVECTOR4 directionToLight(-0.57f, 0.57f, -0.57f, 0.f);

	m_pToonConstTable->SetVector(
		Device,
		m_hToonLightDir,
		&directionToLight
	);

	m_pToonConstTable->SetDefaults(Device);
	m_pOutlineConstTable->SetDefaults(Device);

	// 투영 행렬
	D3DXMatrixPerspectiveFovLH(
		&m_matProj, D3DX_PI * 0.25f,
		(float)g_iWinSizeX / (float)g_iWinSizeY, 1.f, 1000.f
	);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	for (int i = 0; i < 4; i++)
	{
		d3d::Release<ID3DXMesh*>(m_pMeshes[i]);
		d3d::Delete<CSilhouetteEdge*>(m_pMeshOutline[i]);
	}
	d3d::Release<IDirect3DTexture9*>(m_pShadeTex);
	d3d::Release<IDirect3DVertexShader9*>(m_pToonShader);
	d3d::Release<ID3DXConstantTable*>(m_pToonConstTable);
	d3d::Release<IDirect3DVertexShader9*>(m_pOutlineShader);
	d3d::Release<ID3DXConstantTable*>(m_pOutlineConstTable);
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
	D3DXVECTOR3 position(cosf(angle) * 7.f, height, sinf(angle) * 7.f);
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

		// 카툰 그리기
		Device->SetVertexShader(m_pToonShader);
		Device->SetTexture(0, m_pShadeTex);
		
		D3DXMATRIX worldView;
		D3DXMATRIX worldViewProj;
		for (int i = 0; i < 4; ++i)
		{
			worldView = m_matWorld[i] * m_matView;
			worldViewProj = m_matWorld[i] * m_matView * m_matProj;

			m_pToonConstTable->SetMatrix(
				Device,
				m_hToonWorldView,
				&worldView
			);

			m_pToonConstTable->SetMatrix(
				Device,
				m_hToonWorldViewProj,
				&worldViewProj
			); 

			m_pToonConstTable->SetVector(
				Device,
				m_hToonColor,
				&m_vMeshColor[i]
			);

			m_pMeshes[i]->DrawSubset(0);
		}

		// 외곽선 그리기
		Device->SetVertexShader(m_pOutlineShader);
		Device->SetTexture(0, 0);
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // -> 외곽선 사각형이 펼쳐지면서 CW, CCW 구분이 힘들어져서
															  //    이때 필요한 외곽선이 컬링으로 지워지면 안되니깐 다 그림

		for (int i = 0; i < 4; ++i)
		{
			worldView = m_matWorld[i] * m_matView;

			m_pOutlineConstTable->SetMatrix(
				Device,
				m_hOutlineWorldView,
				&worldView
			);

			m_pOutlineConstTable->SetMatrix(
				Device,
				m_hOutlineProjHandle,
				&m_matProj
			);
			
			m_pMeshOutline[i]->Render();
		}
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // 다시 돌리기

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
