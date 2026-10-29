#include "pch.h"
#include "Quad.h"

CQuad::CQuad()
{
	m_vtxList.emplace_back(-1.f, -1.f, 1.25f, 0.f, 0.f, -1.f, 0.f, 3.f);
	m_vtxList.emplace_back(-1.f, 1.f, 1.25f, 0.f, 0.f, -1.f, 0.f, 0.f);
	m_vtxList.emplace_back(1.f, 1.f, 1.25f, 0.f, 0.f, -1.f, 3.f, 0.f);

	m_vtxList.emplace_back(-1.f, -1.f, 1.25f, 0.f, 0.f, -1.f, 0.f, 3.f);
	m_vtxList.emplace_back(1.f, 1.f, 1.25f, 0.f, 0.f, -1.f, 3.f, 0.f);
	m_vtxList.emplace_back(1.f, -1.f, 1.25f, 0.f, 0.f, -1.f, 3.f, 3.f);

	D3DXCreateTextureFromFile(Device, L"./Bin/Resource/dx5_logo.bmp", &m_tex);
}

CQuad::~CQuad()
{
}
