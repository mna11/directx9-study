#include "pch.h"
#include "Quad.h"

CQuad::CQuad()
	: m_vb(nullptr), m_device(nullptr)
{
}

CQuad::~CQuad()
{
	d3d::Release<IDirect3DVertexBuffer9*>(m_vb);
}

void CQuad::Initialize(IDirect3DDevice9* device)
{
	m_device = device;

	m_device->CreateVertexBuffer(
		6 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&m_vb,
		0
	);

	Vertex* v;
	m_vb->Lock(0, 0, (void**)&v, 0);
	v[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	m_vb->Unlock();
}

CQuad* CQuad::Create(IDirect3DDevice9* device)
{
	CQuad* pInstance = new CQuad;
	pInstance->Initialize(device);

	return pInstance;
}

void CQuad::Render(D3DXMATRIX* worldMatrix, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex)
{
	if (worldMatrix)
		m_device->SetTransform(D3DTS_WORLD, worldMatrix);
	if (mtrl)
		m_device->SetMaterial(mtrl);
	if (tex)
		m_device->SetTexture(0, tex);


	m_device->SetStreamSource(0, m_vb, 0, sizeof(Vertex));
	m_device->SetFVF(Vertex::FVF);
	m_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
}

