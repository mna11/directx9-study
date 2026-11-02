#include "pch.h"
#include "Mesh.h"

CMesh::CMesh()
{
	// 빈 메쉬 생성
	HRESULT hr{ 0 };
	hr = D3DXCreateMeshFVF(
		12,
		24,
		D3DXMESH_MANAGED,
		Vertex::FVF,
		Device,
		&m_Mesh
	);
	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXCreateMeshFVF() - FAILED", 0, 0);
		return;
	}

	// 버텍스, 인덱스, 속성(서브셋) 설정
	Vertex* v = 0;
	m_Mesh->LockVertexBuffer(0, (void**)&v);
#pragma region Vertex
	// fill in the front face vertex data
	v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[1] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[2] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	v[3] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	// fill in the back face vertex data
	v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[5] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[7] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// fill in the top face vertex data
	v[8] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[9] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[10] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[11] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	// fill in the bottom face vertex data
	v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[13] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[15] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// fill in the left face vertex data
	v[16] = Vertex(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[17] = Vertex(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[18] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// fill in the right face vertex data
	v[20] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[21] = Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[22] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[23] = Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
#pragma endregion Vertex
	m_Mesh->UnlockVertexBuffer();

	WORD* i = 0;
	m_Mesh->LockIndexBuffer(0, (void**)&i);
#pragma region Index 
	// fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;
#pragma endregion Index
	m_Mesh->UnlockIndexBuffer();

	DWORD* a = 0;
	m_Mesh->LockAttributeBuffer(0, &a);
#pragma region Attr
	for (int i = 0; i < 4; i++)
		a[i] = 0;
	for (int i = 4; i < 8; i++)
		a[i] = 1;
	for (int i = 8; i < 12; i++)
		a[i] = 2;
#pragma endregion Attr
	m_Mesh->UnlockAttributeBuffer();

	// 메쉬 최적화를 위해 속성 테이블 생성 (서브셋 정렬)
	vector<DWORD> vecAdjacencyBuffer(m_Mesh->GetNumFaces() * 3);
	m_Mesh->GenerateAdjacency(0.f, &vecAdjacencyBuffer[0]); // 인접 정보 얻기

	hr = m_Mesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT | 
		D3DXMESHOPT_VERTEXCACHE,
		&vecAdjacencyBuffer[0],
		0, 0, 0
	);

	// 텍스처
	D3DXCreateTextureFromFile(
		Device,
		L"./Bin/Resource/brick0.jpg",
		&m_Textures[0]
	);
	D3DXCreateTextureFromFile(
		Device,
		L"./Bin/Resource/brick1.jpg",
		&m_Textures[1]
	);
	D3DXCreateTextureFromFile(
		Device,
		L"./Bin/Resource/checker.jpg",
		&m_Textures[2]
	);
}

CMesh::~CMesh()
{
	d3d::Release<ID3DXMesh*>(m_Mesh);
	d3d::Release<IDirect3DTexture9*>(m_Textures[0]);
	d3d::Release<IDirect3DTexture9*>(m_Textures[1]);
	d3d::Release<IDirect3DTexture9*>(m_Textures[2]);
}

HRESULT CMesh::Render()
{
	for (int i = 0; i < m_NumSubSets; ++i)
	{
		Device->SetTexture(0, m_Textures[i]);
		m_Mesh->DrawSubset(i);
	}

	return S_OK;
}
