#include "pch.h"
#include "XMesh.h"

CXMesh::CXMesh()
{
	HRESULT hr = 0;

	// 버퍼 선언
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD		 numMtrls = 0;
	
	ID3DXMesh* pSourceMesh;
	hr = D3DXLoadMeshFromX(
		L"./Bin/Resource/bigship1.x",
		D3DXMESH_MANAGED,
		Device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&pSourceMesh
	);

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXLoadMeshFromX() - FAILED", 0, 0);
		return;
	}

	// 마테리얼 추출 & 텍스처 로드
	if (mtrlBuffer != 0 && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
		for (int i = 0; i < numMtrls; ++i)
		{
			// .x 파일은 Ambient 필드가 없어서 기본 색으로 해준다. 
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			m_Mtrl.push_back(mtrls[i].MatD3D);

			if (mtrls[i].pTextureFilename != 0)
			{
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFileA(
					Device,
					mtrls[i].pTextureFilename,
					&tex
				);

				m_Tex.push_back(tex);
			}
			else
			{
				m_Tex.push_back(0);
			}
		}
	}
	d3d::Release<ID3DXBuffer*>(mtrlBuffer);

	// 메시 최적화
	hr = pSourceMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		(DWORD*)adjBuffer->GetBufferPointer(), // 새로 얻은 최적화 후 인접 정보
		0, 0
	);

	if (FAILED(hr))
	{
		::MessageBox(0, L"OptimizeInplace - FAILED", 0, 0);
		return;
	}

	// 프로그레시브 메쉬 생성하기
	hr = D3DXGeneratePMesh(
		pSourceMesh,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0,
		0,
		1,
		D3DXMESHSIMP_FACE,
		&m_PMesh
	);

	d3d::Release<ID3DXMesh*>(pSourceMesh);
	d3d::Release<ID3DXBuffer*>(adjBuffer);

	if (FAILED(hr))
	{
		::MessageBox(0, L"D3DXGeneratePMesh() - FAILED", 0, 0);
		return;
	}

	// 현재 최대 메쉬 개수
	DWORD maxFaces = m_PMesh->GetMaxFaces();
	m_PMesh->SetNumFaces(maxFaces);

}

CXMesh::~CXMesh()
{
	d3d::Release<ID3DXPMesh*>(m_PMesh);
	for (int i = 0; i < m_Tex.size(); ++i)
		d3d::Release<IDirect3DTexture9*>(m_Tex[i]);
}

void CXMesh::Render()
{
	for (int i = 0; i < m_Mtrl.size(); ++i)
	{
		Device->SetMaterial(&m_Mtrl[i]);
		Device->SetTexture(0, m_Tex[i]);
		m_PMesh->DrawSubset(i);

		Device->SetMaterial(&d3d::YELLOW_MTRL);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_PMesh->DrawSubset(i);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}
