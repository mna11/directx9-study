#include "pch.h"
#include "Teapot.h"

CTeapot::CTeapot()
{
	D3DXCreateTeapot(Device, &m_Mesh, 0);
}

CTeapot::~CTeapot()
{
	d3d::Release<ID3DXMesh*>(m_Mesh);
}
