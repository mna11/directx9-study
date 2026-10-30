#include "pch.h"
#include "MainApp.h"

CMainApp::CMainApp()
	: m_VB(0), m_texFloor(0), m_texWall(0), m_texMirror(0),
	m_pTeapot(0)
{
	m_mtrlFloor = d3d::WHITE_MTRL;
	m_mtrlWall = d3d::WHITE_MTRL;
	m_mtrlMirror = d3d::WHITE_MTRL;

	m_mtrlTeapot = d3d::YELLOW_MTRL;

	m_vTeapotPos = { 0.f, 3.f, -7.5f };
}

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	m_mtrlWall.Specular = d3d::WHITE * 0.2f;
	D3DXCreateTeapot(Device, &m_pTeapot, 0);
	Device->CreateVertexBuffer(
		24 * sizeof(Vertex),
		0, // usage
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&m_VB,
		0
	);

	Vertex* v = 0;
	m_VB->Lock(0, 0, (void**)&v, 0);

	// floor
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// wall
	v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Note: We leave gap in middle of walls for mirror

	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// mirror
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	m_VB->Unlock();

	// 텍스처 불러오고 필터 적용
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/checker.jpg", &m_texFloor);
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/brick0.jpg", &m_texWall);
	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/ice.jpg", &m_texMirror);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 빛 설정
	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.f, 1.f, 1.f, 1.f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&lightDir, &color);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	D3DXVECTOR3 pos(-10.f, 3.f, -15.f);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI / 4.f,
		(float)g_iWinSizeX / (float)g_iWinSizeY,
		1.f,
		1000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_VB);
	d3d::Release<IDirect3DTexture9*>(m_texFloor);
	d3d::Release<IDirect3DTexture9*>(m_texWall);
	d3d::Release<IDirect3DTexture9*>(m_texMirror);
	d3d::Release<ID3DXMesh*>(m_pTeapot);
}

void CMainApp::Update(float timeDelta)
{
	static float radius = 20.f;
	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
		m_vTeapotPos.x -= 3.0f * timeDelta;

	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
		m_vTeapotPos.x += 3.0f * timeDelta;

	if (::GetAsyncKeyState(VK_UP) & 0x8000f)
		radius -= 2.0f * timeDelta;

	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
		radius += 2.0f * timeDelta;

	static float angle = (3.0f * D3DX_PI) / 2.0f;

	if (::GetAsyncKeyState('A') & 0x8000f)
		angle -= 0.5f * timeDelta;

	if (::GetAsyncKeyState('S') & 0x8000f)
		angle += 0.5f * timeDelta;

	D3DXVECTOR3 position(cosf(angle) * radius, 3.0f, sinf(angle) * radius);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
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
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x0000000, 1.f, 0L);
		Device->BeginScene();

		RenderScene();
		RenderShadow();
		RenderMirror();

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}

void CMainApp::RenderScene()
{
	// 주전자 그리기
	Device->SetMaterial(&m_mtrlTeapot);
	Device->SetTexture(0, 0);

	Device->SetTexture(0, 0);
	D3DXMATRIX W;
	D3DXMatrixTranslation(&W,
		m_vTeapotPos.x,
		m_vTeapotPos.y,
		m_vTeapotPos.z);

	Device->SetTransform(D3DTS_WORLD, &W);
	m_pTeapot->DrawSubset(0);

	// 나머지 그리기
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);

	Device->SetStreamSource(0, m_VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);

	// 바닥 그리기
	Device->SetMaterial(&m_mtrlFloor);
	Device->SetTexture(0, m_texFloor);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// 벽 그리기
	Device->SetMaterial(&m_mtrlWall);
	Device->SetTexture(0, m_texWall);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

	// 거울 그리기
	Device->SetMaterial(&m_mtrlMirror);
	Device->SetTexture(0, m_texMirror);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
}

void CMainApp::RenderMirror()
{
	//
	// Draw Mirror quad to stencil buffer ONLY.  In this way
	// only the stencil bits that correspond to the mirror will
	// be on.  Therefore, the reflected teapot can only be rendered
	// where the stencil bits are turned on, and thus on the mirror 
	// only.
	//

	// [ 스텐실 테스트 세팅 ] 

  // 스텐실 버퍼 활성화 
	Device->SetRenderState(D3DRS_STENCILENABLE, true);
	// 스텐실 테스트 비교 연산자 설정 - 항상 true 
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	// 스텐실 테스트 참조값 1
	Device->SetRenderState(D3DRS_STENCILREF, 0x1);
	// 마스크로 모든 비트 고려하겠다고 선언
	Device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	// 깊이 테스트나 스텐실 테스트가 실패했다면 기존 스텐실 값을 유지하겠다. (갱신 X)
	// - 깊이 테스트 실패 : 픽셀이 가려짐 (가려진 픽셀 그릴 필요 X)
	// - 스텐실 테스트 실패 : 현재는 ALWAYS나 실패할 일이 없긴 하나 추후를 위해 설정
	Device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	// 깊이 테스트, 스텐실 테스트 모두 통과했다면 스텐실 값을 참조값(0x1)으로 바꾸겠다.
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// [ 스텐실 버퍼에 거울 렌더링 ] 

	// disable writes to the depth and back buffers
	// 스텐실 버퍼에 그리는게 목적이지
	// 후면 버퍼 갱신이 목적이 아니니깐 깊이 버퍼도 기록해주지 않을거임
	Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	// 알파 블렌딩을 키고, 거울의 거울 색에 곱하는 블렌딩 계수를 0, 기존 후면 버퍼 픽셀의 블렌딩 계수를 1로 해준다.
	// 기존 후면 버퍼 픽셀 색 유지
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// draw the mirror to the stencil buffer
	// 스텐실 버퍼에 거울을 그린다.
	// 후면 버퍼는 갱신되지 않음 (위의 설정들 때문에)
	// 그러면 거울은 언제 그리나요? -> 전체 코드를 보면 따로 그림
	Device->SetStreamSource(0, m_VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);
	Device->SetMaterial(&m_mtrlMirror); // 사실 스텐실 버퍼 1만드는 작업이라 
	Device->SetTexture(0, m_texMirror); // mtrl과 tex 작업은 불필요하다.
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

	// re-enable depth writes
	// 거울 그리기가 끝났으므로, 깊이 값 기록을 다시 켜줌
	Device->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// 스텐실 버퍼 내의 가시 픽셀 중 거울에 해당하는 픽셀들은 이제 0x1 값을 가짐


	// ---- [ 반사된 주전자 렌더링 ] ----

	// only draw reflected teapot to the pixels where the mirror
	// was drawn to.
	// 이제 스텐실 값이 참조값(1)인 경우에만(EQUAL) 스텐실 테스트를 통과할 수 있다.
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	// 깊이 테스트와 스텐실 테스트가 전부 통과했을 경우에도 갱신하지 않게 설정한다.
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	// [ 반사의 위치를 잡는 행렬 계산 ]

	// position reflection
	D3DXMATRIX W, T, R;
	// 반사의 기준이 될 평면 세팅
	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	// 반사 행렬 구하기
	D3DXMatrixReflect(&R, &plane);

	// 일단 반사되지 않은 주전자 위치로 이동
	D3DXMatrixTranslation(&T,
		m_vTeapotPos.x,
		m_vTeapotPos.y,
		m_vTeapotPos.z);

	// 기존 주전자 위치로 이동 후, xy-평면 반사 위치로 이동시키는 변환 행렬
	W = T * R;

	// [ 반사된 주전자 그리기 ] 

	// clear depth buffer and blend the reflected teapot with the mirror
	// 거울이 주전자보다 뒤에 있으므로 이대로면 주전자가 거울에 가려져버림
	// 그래서 깊이 버퍼를 초기화해주자 
	// (엥 그러면 기존에는 깊이 테스트를 못하게되는거 아니에요? -> 그래서 RenderMirror는 RenderScene이라는 일반 씬을 다 렌더한 뒤에 실행된다.)
	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	// SRC - 주전자, DEST - 거울
	// 주전자에 거울 색을 블렌딩하기 위한 설정
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// Finally, draw the reflected teapot
	// 주전자에 월드 행렬 적용하고 그리기
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&m_mtrlTeapot);
	Device->SetTexture(0, 0);

	// 반사된 물체는 Winding 순서가 바뀌므로 후면 추려내기 변경하기
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_pTeapot->DrawSubset(0);

	// Restore render states.
	// 설정들 돌리기
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CMainApp::RenderShadow()
{
	Device->SetRenderState(D3DRS_STENCILENABLE, true);
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	Device->SetRenderState(D3DRS_STENCILREF, 0x0);
	Device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR); // increment to 1 -> 처음만 그려준다.

	// 그림자 구하기
	D3DXVECTOR4 lightDirection(0.707f, -0.707f, 0.707f, 0.f);
	D3DXPLANE groundPlane(0.f, -1.f, 0.f, 0.f);

	D3DXMATRIX S;
	D3DXMatrixShadow(&S, &lightDirection, &groundPlane);

	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, m_vTeapotPos.x, m_vTeapotPos.y, m_vTeapotPos.z);

	D3DXMATRIX W = T * S;
	Device->SetTransform(D3DTS_WORLD, &W);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9 mtrl = d3d::InitMtrl(d3d::BLACK, d3d::BLACK, d3d::BLACK, d3d::BLACK, 0.f);
	mtrl.Diffuse.a = 0.5f;

	// z-경합(fighting) 방지
	Device->SetRenderState(D3DRS_ZENABLE, false);

	Device->SetMaterial(&mtrl);
	Device->SetTexture(0, 0);
	m_pTeapot->DrawSubset(0);

	Device->SetRenderState(D3DRS_ZENABLE, true);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_STENCILENABLE, false);
}