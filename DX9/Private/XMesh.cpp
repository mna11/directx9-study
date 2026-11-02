#include "pch.h"
#include "XMesh.h"
#include "BoundingVolume.h"

CXMesh::CXMesh()
{
	HRESULT hr = 0;

	// 버퍼 선언
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD		 numMtrls = 0;

	hr = D3DXLoadMeshFromX(
		L"./Bin/Resource/bigship1.x",
		D3DXMESH_MANAGED,
		Device,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&m_Mesh
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
	hr = m_Mesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0
	);
	d3d::Release<ID3DXBuffer*>(adjBuffer);
	if (FAILED(hr))
	{
		::MessageBox(0, L"OptimizeInplace - FAILED", 0, 0);
		return;
	}

	// Bounding Volume 생성
	m_pBoundingVolume = CBoundingVolume::Create(CBoundingVolume::BOX);
	m_pBoundingVolume->ComputeBoundingBox(m_Mesh);
	m_pBoundingVolume->ComputeBoundingSphere(m_Mesh);
}

CXMesh::~CXMesh()
{
	d3d::Release<ID3DXMesh*>(m_Mesh);
	for (int i = 0; i < m_Tex.size(); ++i)
		d3d::Release<IDirect3DTexture9*>(m_Tex[i]);

	delete m_pBoundingVolume;
}

void CXMesh::Render()
{
	for (int i = 0; i < m_Mtrl.size(); ++i)
	{
		Device->SetMaterial(&m_Mtrl[i]);
		Device->SetTexture(0, m_Tex[i]);
		m_Mesh->DrawSubset(i);
	}

	m_pBoundingVolume->Render();
}
