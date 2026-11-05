#include "pch.h"
#include "SilhouetteEdge.h"

const DWORD MeshVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;


CSilhouetteEdge::CSilhouetteEdge(ID3DXMesh* mesh, ID3DXBuffer* adjBuffer)
{
	GenEdgeVertices(mesh, adjBuffer);	// 외곽선 정점 만들기
	GenEdgeIndices(mesh);				// 외곽선 정점들 삼각형으로 연결하기 
	CreateVertexDeclaration();			// EdgeVertex의 메모리 배치를 셰이더에 설명할 선언을 만든다.
}

CSilhouetteEdge::~CSilhouetteEdge()
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_VB);
	d3d::Release<IDirect3DIndexBuffer9*>(m_IB);
	d3d::Release<IDirect3DVertexDeclaration9*>(m_Decl);
}

void CSilhouetteEdge::Render()
{
	Device->SetVertexDeclaration(m_Decl); // EdgeVertex를 어떻게 해석할지 넘김
	Device->SetStreamSource(0, m_VB, 0, sizeof(EdgeVertex)); // 스트림에 정점 버퍼 연결
	Device->SetIndices(m_IB);								 // 인덱스 버퍼 연결

	// 그리기
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVerts, 0, m_iNumFaces);
}

bool CSilhouetteEdge::CreateVertexDeclaration()
{
	HRESULT hr = { 0 };
	D3DVERTEXELEMENT9 decl[] =
	{
		// stream, offset, type, method, usage, usageIndex
		{0, 0,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0}, // 정점 확장 방향
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1},	// 현재 면 법선
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2},	// 이웃 면 법선
		D3DDECL_END()
	};

	hr = Device->CreateVertexDeclaration(decl, &m_Decl);
	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexDeclaration() - FAILED", 0, 0);
		return false;
	}

	return true;
}

void CSilhouetteEdge::GetFaceNormal(ID3DXMesh* mesh, DWORD faceIndex, D3DXVECTOR3* faceNormal)
{
	// 원본 메쉬의 정점 데이터와 인덱스 데이터에 접근
	MeshVertex* vertices = { nullptr };
	mesh->LockVertexBuffer(0, (void**)&vertices);
	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	// 삼각형 인덱스 얻기 - 삼각형 하나는 인덱스 3개로 구성됨 (정점 번호)
	WORD index0 = indices[faceIndex * 3];
	WORD index1 = indices[faceIndex * 3 + 1];
	WORD index2 = indices[faceIndex * 3 + 2];

	// 삼각형 위치 얻기 - 정점 번호를 이용해 실제 위치 얻기
	D3DXVECTOR3 v0 = vertices[index0].position;
	D3DXVECTOR3 v1 = vertices[index1].position;
	D3DXVECTOR3 v2 = vertices[index2].position;

	// 면 법선 계산
	D3DXVECTOR3 edge0, edge1;
	edge0 = v1 - v0;
	edge1 = v2 - v0;
	D3DXVec3Cross(faceNormal, &edge0, &edge1);
	D3DXVec3Normalize(faceNormal, faceNormal);

	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void CSilhouetteEdge::GetFaceNormals(ID3DXMesh* mesh, ID3DXBuffer* adjBuffer, D3DXVECTOR3* currentFaceNormal, D3DXVECTOR3 adjFaceNormal[3], DWORD faceIndex)
{
	// 원본 버퍼에 접근
	MeshVertex* vertices = { nullptr };
	mesh->LockVertexBuffer(0, (void**)&vertices);
	WORD* indices = { nullptr };
	mesh->LockIndexBuffer(0, (void**)&indices);
	// 인접 정보는 면마다 DWORD 3개씩 저장되어 있다.
	DWORD* adj = (DWORD*)adjBuffer->GetBufferPointer();

	// 면 법선 얻기
	GetFaceNormal(mesh, faceIndex, currentFaceNormal);

	// 인접 면 구하기
	WORD faceIndex0 = adj[faceIndex * 3];     // - v0 - v1에 붙어있는 면
	WORD faceIndex1 = adj[faceIndex * 3 + 1]; // - v1 - v2에 붙어있는 면
	WORD faceIndex2 = adj[faceIndex * 3 + 2]; // - v2 - v0에 붙어있는 면

	D3DXVECTOR3 faceNormal0, faceNormal1, faceNormal2;

	if (faceIndex0 != USHRT_MAX) // 이웃 면이 없으면 0xffffffff를 줌
	{
		// 이웃 삼각형의 정점 번호 3개 얻기
		WORD i0 = indices[faceIndex0 * 3];
		WORD i1 = indices[faceIndex0 * 3 + 1];
		WORD i2 = indices[faceIndex0 * 3 + 2];

		// 정점 번호로 실제 위치 구하기
		D3DXVECTOR3 v0 = vertices[i0].position;
		D3DXVECTOR3 v1 = vertices[i1].position;
		D3DXVECTOR3 v2 = vertices[i2].position;

		// 법선 구하기
		D3DXVECTOR3 edge0 = v1 - v0;
		D3DXVECTOR3 edge1 = v2 - v0;
		D3DXVec3Cross(&faceNormal0, &edge0, &edge1);
		D3DXVec3Normalize(&faceNormal0, &faceNormal0);
	}
	else
	{
		// 이웃면이 없다면 외곽선 처리를 할 수 있도록 반대로 함
		faceNormal0 = -(*currentFaceNormal);
	}

	if (faceIndex1 != USHRT_MAX) // 이웃 면이 없으면 0xffffffff를 줌
	{
		WORD i0 = indices[faceIndex1 * 3];
		WORD i1 = indices[faceIndex1 * 3 + 1];
		WORD i2 = indices[faceIndex1 * 3 + 2];

		D3DXVECTOR3 v0 = vertices[i0].position;
		D3DXVECTOR3 v1 = vertices[i1].position;
		D3DXVECTOR3 v2 = vertices[i2].position;

		D3DXVECTOR3 edge0 = v1 - v0;
		D3DXVECTOR3 edge1 = v2 - v0;
		D3DXVec3Cross(&faceNormal1, &edge0, &edge1);
		D3DXVec3Normalize(&faceNormal1, &faceNormal1);
	}
	else
	{
		faceNormal1 = -(*currentFaceNormal);
	}

	if (faceIndex2 != USHRT_MAX) // 이웃 면이 없으면 0xffffffff를 줌
	{
		WORD i0 = indices[faceIndex2 * 3];
		WORD i1 = indices[faceIndex2 * 3 + 1];
		WORD i2 = indices[faceIndex2 * 3 + 2];

		D3DXVECTOR3 v0 = vertices[i0].position;
		D3DXVECTOR3 v1 = vertices[i1].position;
		D3DXVECTOR3 v2 = vertices[i2].position;

		D3DXVECTOR3 edge0 = v1 - v0;
		D3DXVECTOR3 edge1 = v2 - v0;
		D3DXVec3Cross(&faceNormal2, &edge0, &edge1);
		D3DXVec3Normalize(&faceNormal2, &faceNormal2);
	}
	else
	{
		faceNormal2 = -(*currentFaceNormal);
	}

	// 넘겨주기
	adjFaceNormal[0] = faceNormal0;
	adjFaceNormal[1] = faceNormal1;
	adjFaceNormal[2] = faceNormal2;

	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void CSilhouetteEdge::GenEdgeVertices(ID3DXMesh* mesh, ID3DXBuffer* adjBuffer)
{
	// 1면당 3 모서리, 모서리 당 4 Vertex;
	m_iNumVerts = mesh->GetNumFaces() * 3 * 4;
	Device->CreateVertexBuffer(
		m_iNumVerts * sizeof(EdgeVertex),
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&m_VB,
		0
	);

	MeshVertex* vertices = { nullptr };
	mesh->LockVertexBuffer(0, (void**)&vertices);
	WORD* indices = { nullptr };
	mesh->LockIndexBuffer(0, (void**)&indices);
	
	EdgeVertex* edgeVertices = { nullptr };
	m_VB->Lock(0, 0, (void**)&edgeVertices, 0);

	// 원본 메쉬의 모든 삼각형을 순회한다.
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
	{
		D3DXVECTOR3 currentFaceNormal;
		D3DXVECTOR3 adjFaceNormals[3];

		GetFaceNormals(mesh, adjBuffer, &currentFaceNormal, adjFaceNormals, i);

		WORD index0 = indices[i * 3];		// 정점 번호 구하기
		WORD index1 = indices[i * 3 + 1];
		WORD index2 = indices[i * 3 + 2];

		MeshVertex v0 = vertices[index0];	// 정점 번호로 실제 위치 구하기
		MeshVertex v1 = vertices[index1];
		MeshVertex v2 = vertices[index2];

		EdgeVertex A0, B0, C0, D0;

		// v0-v1 선분 
		A0.position = v0.position;
		A0.normal = { 0.f, 0.f ,0.f };
		A0.faceNormal1 = currentFaceNormal;
		A0.faceNormal2 = adjFaceNormals[0];

		B0.position = v1.position;
		B0.normal = { 0.f, 0.f ,0.f };
		B0.faceNormal1 = currentFaceNormal;
		B0.faceNormal2 = adjFaceNormals[0];

		C0 = A0;
		C0.normal = v0.normal;

		D0 = B0;
		D0.normal = v1.normal;

		*edgeVertices = A0; ++edgeVertices;
		*edgeVertices = B0; ++edgeVertices;
		*edgeVertices = C0; ++edgeVertices;
		*edgeVertices = D0; ++edgeVertices;

		// v1-v2 선분 
		EdgeVertex A1, B1, C1, D1;
		A1.position = v1.position;
		A1.normal = { 0.f, 0.f ,0.f };
		A1.faceNormal1 = currentFaceNormal;
		A1.faceNormal2 = adjFaceNormals[1];

		B1.position = v2.position;
		B1.normal = { 0.f, 0.f ,0.f };
		B1.faceNormal1 = currentFaceNormal;
		B1.faceNormal2 = adjFaceNormals[1];

		C1 = A1;
		C1.normal = v1.normal;

		D1 = B1;
		D1.normal = v2.normal;

		*edgeVertices = A1; ++edgeVertices;
		*edgeVertices = B1; ++edgeVertices;
		*edgeVertices = C1; ++edgeVertices;
		*edgeVertices = D1; ++edgeVertices;

		// v0 - v2 선분
		EdgeVertex A2, B2, C2, D2;

		A2.position = v0.position;
		A2.normal = D3DXVECTOR3(0.f, 0.f, 0.f);
		A2.faceNormal1 = currentFaceNormal;
		A2.faceNormal2 = adjFaceNormals[2];

		B2.position = v2.position;
		B2.normal = D3DXVECTOR3(0.f, 0.f, 0.f);
		B2.faceNormal1 = currentFaceNormal;
		B2.faceNormal2 = adjFaceNormals[2];

		C2 = A2;
		C2.normal = v0.normal;

		D2 = B2; 
		D2.normal = v2.normal;

		*edgeVertices = A2; ++edgeVertices;
		*edgeVertices = B2; ++edgeVertices;
		*edgeVertices = C2; ++edgeVertices;
		*edgeVertices = D2; ++edgeVertices;
	}

	m_VB->Unlock();
	mesh->UnlockVertexBuffer();
	mesh->UnlockIndexBuffer();
}

void CSilhouetteEdge::GenEdgeIndices(ID3DXMesh* mesh)
{
	// 면 수 * 3이 모서리
	DWORD numEdges = mesh->GetNumFaces() * 3; 
	// 각 모서리는 사각형으로 두개의 삼각형을 가지고 있음
	m_iNumFaces = numEdges * 2;

	Device->CreateIndexBuffer(
		numEdges * 6 * sizeof(WORD), // 엣지당 2개 삼각형 * 삼각형당 3개의 점
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_IB,
		0
	);

	// 인덱스 채우기
	WORD* indices = { nullptr };
	m_IB->Lock(0, 0, (void**)&indices, 0);

	// numEdges는 사실 사각형 개수와 동일함
	for (UINT i = 0; i < numEdges; ++i)
	{
		// i번째 사각형은 정점 4개를 사용함
		// i = 0 -> 0 1 2 3
		// i = 1 -> 4 5 6 7
		indices[i * 6]		= i * 4 + 0; //A 
		indices[i * 6 + 1]	= i * 4 + 1; //B
		indices[i * 6 + 2]	= i * 4 + 2; //C
		indices[i * 6 + 3]	= i * 4 + 1; //B
		indices[i * 6 + 4]	= i * 4 + 3; //D
		indices[i * 6 + 5]	= i * 4 + 2; //C
	}
	m_IB->Unlock();
}
