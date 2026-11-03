#include "pch.h"
#include "Terrain.h"

const DWORD CTerrain::TerrainVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

CTerrain::CTerrain(wstring heightmapFileName, int numVertsPerRow, int numVertsPerCol, int cellSpacing, float heightScale)
{
	m_NumVertsPerRow = numVertsPerRow;
	m_NumVertsPerCol = numVertsPerCol;
	m_CellSpacing = cellSpacing;

	m_NumCellsPerRow = m_NumVertsPerRow - 1;
	m_NumCellsPerCol = m_NumVertsPerCol - 1;

	m_Width = m_NumCellsPerRow * m_CellSpacing;
	m_Depth = m_NumCellsPerCol * m_CellSpacing;

	m_NumVertices = m_NumVertsPerRow * m_NumVertsPerCol;
	m_NumTriangles = m_NumCellsPerRow * m_NumCellsPerCol * 2;

	m_HeightScale = heightScale;

	// heightmap 로드
	if (!ReadRawFile(heightmapFileName))
	{
		::MessageBox(0, L"ReadRawFile - Failed", 0, 0);
		::PostQuitMessage(0);
	}

	// Scale Height
	for (int i = 0; i < m_Heightmap.size(); ++i)
		m_Heightmap[i] *= m_HeightScale;

	// 정점 계산
	if (!ComputeVertices())
	{
		::MessageBox(0, L"ComputeVertices - Failed", 0, 0);
		::PostQuitMessage(0);
	}

	// 인덱스 계산
	if (!ComputeIndices())
	{
		::MessageBox(0, L"ComputeIndices - Failed", 0, 0);
		::PostQuitMessage(0);
	}
}

CTerrain::~CTerrain()
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_VB);
	d3d::Release<IDirect3DIndexBuffer9*>(m_IB);
	d3d::Release<IDirect3DTexture9*>(m_Tex);
}

int CTerrain::GetHeightMapEntry(int row, int col)
{
	return m_Heightmap[row * m_NumVertsPerRow + col];
}

void CTerrain::SetHeightMapEntry(int row, int col, int value)
{
	m_Heightmap[row * m_NumVertsPerRow + col] = value;
}

// 지형 위의 임의의 위치 (x, z)에서 높이 y를 계산하는 함수이다.
// 높이맵에는 정점 위치만 높이로 저장되어 있으므로 그걸 계산해야 된다.
float CTerrain::GetHeight(float x, float z)
{
	// 지형의 시작점을 기준으로 좌표를 바꾼다.
	// 왼쪽 상단 기준으로 맞춤
	x = ((float)m_Width / 2.f) + x;
	z = ((float)m_Depth / 2.f) - z;

	// x와 z가 몇 열 몇 행의 셀에 위치하는지 알기 위해 셀 간격을 1로 환산
	x /= (float)m_CellSpacing;
	z /= (float)m_CellSpacing;
	float col = ::floorf(x);
	float row = ::floorf(z);

	// 해당 셀을 둘러싼 네 정점의 높이를 가져온다.
	float A = GetHeightMapEntry(row, col);
	float B = GetHeightMapEntry(row, col + 1);
	float C = GetHeightMapEntry(row + 1, col);
	float D = GetHeightMapEntry(row + 1, col + 1);

	// 셀 안에서의 위치를 구함
	// dx, dz는 현재 셀 안에서 오른쪽과 아래쪽으로 얼마나 이동했는지를 나타냄
	float dx = x - col;
	float dz = z - row;

	float height = 0.f;
	// ABC에 있다.
	if (dz < 1.f - dx)
	{
		float uy = B - A;
		float vy = C - A;
		height = A + d3d::Lerp(0.f, uy, dx) + d3d::Lerp(0.f, vy, dz);
	}
	// CBD에 있다.
	else 
	{
		float uy = C - D;
		float vy = B - D; 
		height = D + d3d::Lerp(0.f, uy, 1.f - dx) + d3d::Lerp(0.f, vy, 1.f - dz);
	}

	return height;
}

bool CTerrain::LoadTexture(wstring fileName)
{
	HRESULT hr = 0;
	hr = D3DXCreateTextureFromFile(Device, fileName.c_str(), &m_Tex);
	if (FAILED(hr))
		return false;
	return true;
}

// 높이맵을 이용해 지형용 텍스처를 직접 만드는 함수
// 지형의 색과 명암을 만든다.
bool CTerrain::GenTexture(D3DXVECTOR3* directionToLight)
{
	// 절차적 텍스처 생성
	// 1. 빈 텍스처 생성
	// 2. 높이에 따라 색칠
	// 3. 조명에 따른 밝기 반영
	// 4. 축소된 텍스처들인 밉맵 생성

	HRESULT hr = 0;

	// 지형의 셀 하나에 텍셀 하나를 대응
	// - 셀인 사각형 하나를 대응하는 텍셀(픽셀 하나)로 채우겠다.
	int texWidth = m_NumCellsPerRow;
	int texHeight = m_NumCellsPerCol;

	// 색을 기록할 빈 텍스처 만들기
	hr = D3DXCreateTexture(
		Device,
		texWidth, texHeight, // 요청하는 텍스트 만들기
		0,					 // 전체 밉맵 단계의 공간 생성 - 메모리만 할당해둔 상태
		0,					 // 특별한 사용 옵션 없음
		D3DFMT_X8R8G8B8,     // 텍셀 하나에 4바이트 사용
		D3DPOOL_MANAGED, &m_Tex // D3D가 관리하는 리소스, 텍스처를 받을 곳
	);
	if (FAILED(hr))
		return false;

	// 생성된 텍스처가 포맷이 맞는지 확인
	D3DSURFACE_DESC textureDesc;
	m_Tex->GetLevelDesc(0, &textureDesc); 
	if (textureDesc.Format != D3DFMT_X8R8G8B8)
		return false;

	// 텍스처 메모리에 접근
	// - pBits : 색을 기록할 메모리의 시작 주소
	// - Pitch : 한 행의 시작에서 다음 행의 시작까지 이동할 바이트 수
	D3DLOCKED_RECT lockedRect;

	m_Tex->LockRect(
		0,			// 밈맵 레벨 0
		&lockedRect,// 접글한 메모리 정보
		0,			// 전체 영역
		0			// 특별한 옵션 없음
	);

	// Surface라 LockRect로 진행
	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for (int i = 0; i < texHeight; ++i)
	{
		for (int j = 0; j < texWidth; ++j)
		{
			D3DXCOLOR c;
			// m_HeightScale로 나눠주는 이유는 간단함
			// - 원래 높이는 0 ~ 255 사이인데, 우리가 Scale만큼 곱해줬었음
			//   그래서 다시 0 ~ 255로 돌리는 작업
			float height = (float)GetHeightMapEntry(i, j) / m_HeightScale;

			if (height < 42.5f)  c = d3d::BEACH_SAND;
			else if (height < 85.f) c = d3d::LIGHT_YELLOW_GREEN;
			else if (height < 127.5f) c = d3d::PUREGREEN;
			else if (height < 170.f) c = d3d::DARK_YELLOW_GREEN;
			else if (height < 212.5f) c = d3d::DARKBROWN;
			else					  c = d3d::WHITE;

			// 계산한 색을 텍스처에 기록한다.
			// 텍스처의 i행 j열에 색 c를 넣는다. 
			// - 대괄호 안 숫자가 1 늘면 메모리는 4바이트 이동함
			// - Pitch가 한 행의 시작에서 다음 행의 시작까지의 바이트 수
			// - 따라서 Pitch / 4를 해준 것이다.
			imageData[i * lockedRect.Pitch / 4 + j] = (D3DCOLOR)c;
			// 그런데 왜 i * texWidth + j로 안하고 이렇게 했을까? 
			// - Surface에서는 하드웨어 구현에 따라서 Pitch = texWidth * 4보다 클 수 있다. 

		}
	}
	m_Tex->UnlockRect(0);

	// 조명 반영
	if (!LightTerrain(directionToLight))
	{
		::MessageBox(0, L"LightTerrain() - FAILED", 0, 0);
		return false;
	}

	// 나머지 밉맵 이미지 생성
	hr = D3DXFilterTexture(
		m_Tex,
		0,
		0,
		D3DX_DEFAULT
	); 

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXFilterTexture() - FAILED", 0, 0);
		return false;
	}

	return true;
}

bool CTerrain::Render(D3DXMATRIX* world, bool drawTris)
{
	HRESULT hr = 0;

	Device->SetStreamSource(0, m_VB, 0, sizeof(TerrainVertex));

	Device->SetTransform(D3DTS_WORLD, world);
	Device->SetFVF(TerrainVertex::FVF);
	Device->SetIndices(m_IB);

	Device->SetTexture(0, m_Tex);
	// 빛 연산을 우리가 알아서 하고 있기때문에 끈다.
	Device->SetRenderState(D3DRS_LIGHTING, false);

	hr = Device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,
		0, 0,
		m_NumVertices,
		0, m_NumTriangles
	);
	Device->SetRenderState(D3DRS_LIGHTING, true);

	// 삼각형 그리기 플래그 - 와이어 프레임
	if (drawTris)
	{
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		hr = Device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0, 0,
			m_NumVertices,
			0, m_NumTriangles
		);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (FAILED(hr))
		return false;

	return true;
}

bool CTerrain::ReadRawFile(wstring filename)
{
	vector<BYTE> in(m_NumVertices);
	ifstream inFile(filename.c_str(), ios_base::binary);

	if (!inFile)
		return false;

	inFile.read((char*)&in[0], in.size());

	inFile.close();

	m_Heightmap.resize(m_NumVertices);
	for (int i = 0; i < in.size(); ++i)
		m_Heightmap[i] = in[i];

	return true;
}

// 높이맵을 바탕으로 지형의 정점을 만들고, 정점 버퍼에 저장
bool CTerrain::ComputeVertices() 
{
	HRESULT hr = 0;
	hr = Device->CreateVertexBuffer(
		m_NumVertices * sizeof(TerrainVertex),
		D3DUSAGE_WRITEONLY,
		TerrainVertex::FVF,
		D3DPOOL_MANAGED,
		&m_VB,
		0
	);

	if (FAILED(hr))
		return false;

	// 이걸 해주는 이유는?
	// - 지형의 중심을 xz 평면의 (0, 0)에 맞추기 위해서
	int startX = -m_Width / 2;
	int startZ = m_Depth / 2;

	int endX = m_Width / 2;
	int endZ = -m_Depth / 2;

	// 텍스처 좌표의 증가량 계산
	float uCoordIncrementSize = 1.f / (float)m_NumCellsPerRow;
	float vCoordIncrementSize = 1.f / (float)m_NumCellsPerCol;

	// 버퍼에 포인터 받기
	TerrainVertex* v = 0;
	m_VB->Lock(0, 0, (void**)&v, 0);

	// 격자를 순회하면서 정점 만들기
	// - i, j : 몇번째 행과 열인지
	// - x, z : 실제 공간에서의 위치
	// - m_CellSpacing : 이웃한 정점 사이의 거리
	int i = 0;
	for (int z = startZ; z >= endZ; z -= m_CellSpacing)
	{
		int j = 0; 
		for (int x = startX; x <= endX; x += m_CellSpacing)
		{
			int index = i * m_NumVertsPerRow + j;

			v[index] = TerrainVertex(
				(float)x,
				(float)m_Heightmap[index],
				(float)z,
				(float)j * uCoordIncrementSize,
				(float)i * vCoordIncrementSize
			);

			j++;
		}
		i++;
	}

	m_VB->Unlock();

	return true;
}

bool CTerrain::ComputeIndices() 
{
	HRESULT hr = 0;
	hr = Device->CreateIndexBuffer(
		m_NumTriangles * 3 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_IB,
		0
	);
	if (FAILED(hr))
		return false;

	WORD* indices = 0;
	m_IB->Lock(0, 0, (void**)&indices, 0);

	int baseIndex = 0;

	for (int i = 0; i < m_NumCellsPerCol; ++i)
	{
		for (int j = 0; j < m_NumCellsPerRow; ++j)
		{
			indices[baseIndex] = i * m_NumVertsPerRow + j;
			indices[baseIndex + 1] = i * m_NumVertsPerRow + (j + 1);
			indices[baseIndex + 2] = (i + 1) * m_NumVertsPerRow + j;

			indices[baseIndex + 3] = (i + 1) * m_NumVertsPerRow + j;
			indices[baseIndex + 4] = i * m_NumVertsPerRow + (j + 1);
			indices[baseIndex + 5] = (i + 1) * m_NumVertsPerRow + (j + 1);

			baseIndex += 6;
		}
	}

	m_IB->Unlock();

	return true;
}
bool CTerrain::LightTerrain(D3DXVECTOR3* directionToLight)
{
	HRESULT hr = 0;
	// 텍스처 포맷 확인
	D3DSURFACE_DESC textureDesc;
	m_Tex->GetLevelDesc(0, &textureDesc);
	if (textureDesc.Format != D3DFMT_X8R8G8B8)
		return false;

	D3DLOCKED_RECT lockedRect;
	m_Tex->LockRect(
		0,
		&lockedRect,
		0,
		0
	);

	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for (int i = 0; i < textureDesc.Height; ++i)
	{
		for (int j = 0; j < textureDesc.Width; ++j)
		{
			int index = i * lockedRect.Pitch / 4 + j;

			// 우리가 설정했던 컬러에 
			D3DXCOLOR c(imageData[index]);
			c *= ComputeShade(i, j, directionToLight);
			imageData[index] = (D3DCOLOR)c;

			// 이렇게하게 되면 원하는 결과가 나오지 않는다.
			// RGB 성분곱이 아니라 그냥 전체에 곱셈을 해서 그럼
			//imageData[index] *= ComputeShade(i, j, directionToLight);
		}
	}
	m_Tex->UnlockRect(0);
	return true;
}

float CTerrain::ComputeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight) 
{
	float heightA = GetHeightMapEntry(cellRow, cellCol);
	float heightB = GetHeightMapEntry(cellRow, cellCol + 1);
	float heightC = GetHeightMapEntry(cellRow + 1, cellCol);

	// 삼각형 위 벡터 두개
	D3DXVECTOR3 u(m_CellSpacing, heightB - heightA, 0.f);
	D3DXVECTOR3 v(0.f, heightC - heightA, -m_CellSpacing);

	// 노말 찾기
	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &u, &v);
	D3DXVec3Normalize(&n, &n);

	float cosine = D3DXVec3Dot(&n, directionToLight);

	if (cosine < 0.f)
		cosine = 0.f;

	return cosine;
}