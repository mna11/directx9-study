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

	Device->CreateVertexBuffer(
		6 * sizeof(MultiTexVertex),
		D3DUSAGE_WRITEONLY,
		MultiTexVertex::FVF,
		D3DPOOL_MANAGED,
		&m_pQuadVB,
		0
	);

	MultiTexVertex* v = { nullptr };
	m_pQuadVB->Lock(0, 0, (void**)&v, 0);
	v[0] = MultiTexVertex(-10.0f, -10.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[1] = MultiTexVertex(-10.0f, 10.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = MultiTexVertex(10.0f, 10.0f, 5.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = MultiTexVertex(-10.0f, -10.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[4] = MultiTexVertex(10.0f, 10.0f, 5.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = MultiTexVertex(10.0f, -10.0f, 5.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	m_pQuadVB->Unlock();

	ID3DXBuffer* shader = { nullptr };
	ID3DXBuffer* errorBuffer = { nullptr };

	hr = D3DXCompileShaderFromFile(
		L"./Bin/Resource/Shader/ps_alphablend.hlsl",
		0,
		0,
		"main",
		"ps_2_0",
		D3DXSHADER_DEBUG,
		&shader,
		&errorBuffer,
		&m_pMultiTexCT
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

	hr = Device->CreatePixelShader(
		(DWORD*)shader->GetBufferPointer(),
		&m_pMultiTexPS
	);

	if (FAILED(hr))
	{
		::MessageBox(0, L"CreatePixelShader() - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(shader);

	// 텍스처 로드
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/crate.bmp", &m_pBaseTex);
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/spotlight.bmp", &m_pSpotLightTex);
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/text.bmp", &m_pStringTex);

	// 투영 행렬
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f,
		(float)g_iWinSizeX / (float)g_iWinSizeY,
		1.f,
		1000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	// 빛 비활성화
	Device->SetRenderState(D3DRS_LIGHTING, false);

	// 핸들 얻기
	m_hBaseTex = m_pMultiTexCT->GetConstantByName(0, "BaseTex");
	m_hSpotLightTex = m_pMultiTexCT->GetConstantByName(0, "SpotLightTex");
	m_hStringTex = m_pMultiTexCT->GetConstantByName(0, "StringTex");

	// Desc 설정
	UINT count = 1;
	m_pMultiTexCT->GetConstantDesc(m_hBaseTex, &m_descBaseTex, &count);
	m_pMultiTexCT->GetConstantDesc(m_hSpotLightTex, &m_descSpotLightTex, &count);
	m_pMultiTexCT->GetConstantDesc(m_hStringTex, &m_descStringTex, &count);
	m_pMultiTexCT->SetDefaults(Device);


	// base Tex
	Device->SetTexture(m_descBaseTex.RegisterIndex, m_pBaseTex);
	Device->SetSamplerState(m_descBaseTex.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(m_descBaseTex.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(m_descBaseTex.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// spotLight Tex
	Device->SetTexture(m_descSpotLightTex.RegisterIndex, m_pSpotLightTex);
	Device->SetSamplerState(m_descSpotLightTex.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(m_descSpotLightTex.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(m_descSpotLightTex.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// string Tex
	Device->SetTexture(m_descStringTex.RegisterIndex, m_pStringTex);
	Device->SetSamplerState(m_descStringTex.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(m_descStringTex.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(m_descStringTex.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_pQuadVB);

	d3d::Release<IDirect3DTexture9*>(m_pBaseTex);
	d3d::Release<IDirect3DTexture9*>(m_pSpotLightTex);
	d3d::Release<IDirect3DTexture9*>(m_pStringTex);

	d3d::Release<IDirect3DPixelShader9*>(m_pMultiTexPS);
	d3d::Release<ID3DXConstantTable*>(m_pMultiTexCT);
}

void CMainApp::Update(float timeDelta)
{
	static float angle = (3.f * D3DX_PI) / 2.f;
	static float radius = 20.f;

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000)
		angle -= 0.5f * timeDelta;
	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000)
		angle += 0.5f * timeDelta;
	if (::GetAsyncKeyState(VK_UP) & 0x8000)
		radius -= 2.f * timeDelta;
	if (::GetAsyncKeyState(VK_DOWN) & 0x8000)
		radius += 2.f * timeDelta;

	D3DXVECTOR3 pos(cosf(angle) * radius, 0.f, sinf(angle) * radius);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V, &pos, &target, &up
	);
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

		Device->SetPixelShader(m_pMultiTexPS);
		Device->SetFVF(MultiTexVertex::FVF);
		Device->SetStreamSource(0, m_pQuadVB, 0, sizeof(MultiTexVertex));

		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

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