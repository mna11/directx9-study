#include "pch.h"
#include "BoundingVolume.h"

CBoundingVolume::~CBoundingVolume()
{
	d3d::Release<ID3DXMesh*>(m_SphereMesh);
	d3d::Release<ID3DXMesh*>(m_BoxMesh);
}

bool CBoundingVolume::isPointInside(D3DXVECTOR3& p)
{
	switch (m_eType)
	{
	case BOX:
		if (p.x >= m_vMin.x && p.y >= m_vMin.y && p.z >= m_vMin.z &&
			p.x <= m_vMax.x && p.y <= m_vMax.y && p.z <= m_vMax.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	case SPHERE:
		if (((p.x - m_vCenter.x) * (p.x - m_vCenter.x)
			+ (p.y - m_vCenter.y) * (p.y - m_vCenter.y)
			+ (p.z - m_vCenter.z) * (p.z - m_vCenter.z)) <= m_fRadius * m_fRadius)
		{
			return true;
		}
		else
		{
			return false;
		}

		break;
	}
}

bool CBoundingVolume::ComputeBoundingSphere(ID3DXMesh* mesh)
{
	HRESULT hr = 0;
	BYTE* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&m_vCenter,
		&m_fRadius
	);

	mesh->UnlockVertexBuffer();

	if (FAILED(hr))
		return false;

	// 메쉬 생성
	hr = D3DXCreateSphere(
		Device,
		m_fRadius,
		20,
		20,
		&m_SphereMesh,
		0
	);

	if (FAILED(hr))
		return false;

	return true;
}

bool CBoundingVolume::ComputeBoundingBox(ID3DXMesh* mesh)
{
	HRESULT hr = 0;

	BYTE* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&m_vMin,
		&m_vMax
	);

	mesh->UnlockVertexBuffer();
	if (FAILED(hr))
		return false; 
	
	// 메쉬 생성

	hr = D3DXCreateBox(
		Device,
		m_vMax.x - m_vMin.x,
		m_vMax.y - m_vMin.y,
		m_vMax.z - m_vMin.z,
		&m_BoxMesh, 0
	);

	if (FAILED(hr))
		return false;

	return true;
}

CBoundingVolume* CBoundingVolume::Create(BOUND_TYPE eType)
{
	CBoundingVolume* pInstance = new CBoundingVolume;
	pInstance->m_eType = eType;
	return pInstance;
}

void CBoundingVolume::Render()
{
	D3DMATERIAL9 blue = d3d::BLUE_MTRL;
	blue.Diffuse.a = 0.4f;

	Device->SetMaterial(&blue);
	Device->SetTexture(0, 0); // 텍스처 비활성화

	// 알파블렌딩 활성화
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	switch (m_eType)
	{
	case BOX:
		if (m_BoxMesh)
			m_BoxMesh->DrawSubset(0);
		break;
	case SPHERE:
		if (m_SphereMesh)
			m_SphereMesh->DrawSubset(0);
		break;
	}

	// 알파블렌딩 비활성화
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}
