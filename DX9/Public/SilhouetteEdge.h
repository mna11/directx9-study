#pragma once

/*
* 원본 메쉬의 각 모서리(edge)에 퇴축 사각형을 만들고 셰이더가 실루엣으로 판정한 사각형만 펼쳐서 그리는 역할을 함
*/

// 외곽선용 정점
struct EdgeVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;			// 외곽선 두께를 만들기 위해 정점을 밀어낼 방향
	D3DXVECTOR3 faceNormal1;	// 외곽선을 공유하는 첫 번째 면의 법선
	D3DXVECTOR3 faceNormal2;	// 외곽선을 공유하는 두 번째 면의 법선
};

// 원본 메쉬의 정점
struct MeshVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	static const DWORD FVF;
};

// 실루엣 외곽선을 그리기 위한 정점/인덱스 버퍼를 관리
class CSilhouetteEdge
{
public:
	CSilhouetteEdge(
		ID3DXMesh* mesh,		// 외곽선을 만들 대상 원본 메쉬
		ID3DXBuffer* adjBuffer  // 원본 메쉬의 각 삼각형의 이웃 면 번호가 들어있음
	);
	~CSilhouetteEdge();

	void Render();

private:
	LPDIRECT3DVERTEXBUFFER9			m_VB = { };			// 외곽선 정점 VB
	LPDIRECT3DINDEXBUFFER9			m_IB = { };			// 외곽선 정점 IB
	LPDIRECT3DVERTEXDECLARATION9	m_Decl = { };		// EdgeVertex를 셰이더 입력과 연결하는 정점 선언

	uint32_t m_iNumVerts = {};	// 외곽선 정점 총개수
	uint32_t m_iNumFaces = {};	// 외곽선 삼각형 총개수

private:
	bool CreateVertexDeclaration(); // EdgeVertex에 맞는 정점 선언
	// 특정 삼각형 하나의 면 법선을 계산
	void GetFaceNormal(ID3DXMesh* mesh, DWORD faceIndex, D3DXVECTOR3* faceNormal);
	// 현재 삼각형의 면 법선과 이웃면의 면 법선을 계산
	void GetFaceNormals(ID3DXMesh* mesh, ID3DXBuffer* adjBuffer, D3DXVECTOR3* currentFaceNormal, D3DXVECTOR3 adjFaceNormal[3], DWORD faceIndex);
	// 원본 메쉬의 각 삼각형에서 세 모서리를 꺼내고
	// 모서리마다 EdgeVertex를 4개를 만든다.
	void GenEdgeVertices(ID3DXMesh* mesh, ID3DXBuffer* adjBuffer);
	// 외곽선용 정점 4개를 삼각형 두개로 연결한다.
	void GenEdgeIndices(ID3DXMesh* mesh);
};

