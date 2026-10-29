#include "pch.h"
#include "Teapot.h"

CTeapot::CTeapot()
	: m_mesh(nullptr), m_device(nullptr)
{
}

CTeapot::~CTeapot()
{
	d3d::Release<ID3DXMesh*>(m_mesh);
}

void CTeapot::Initialize(IDirect3DDevice9* device)
{
	m_device = device;
	D3DXCreateTeapot(m_device, &m_mesh, nullptr);
}

CTeapot* CTeapot::Create(IDirect3DDevice9* device)
{
	CTeapot* pInstance = new CTeapot;
	pInstance->Initialize(device);

	return pInstance;
}

void CTeapot::Render(D3DXMATRIX* worldMatrix, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex)
{
	if (worldMatrix)
		m_device->SetTransform(D3DTS_WORLD, worldMatrix);
	if (mtrl)
		m_device->SetMaterial(mtrl);
	if (tex)
		m_device->SetTexture(0, tex);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_mesh->DrawSubset(0);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

