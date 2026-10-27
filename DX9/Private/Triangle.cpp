#include "pch.h"
#include "Triangle.h"

CTriangle::CTriangle()
{ 
	m_vtxList.emplace_back(-1.f, 0.f, 2.f, D3DCOLOR_XRGB(255, 0, 0));
	m_vtxList.emplace_back(0.f, 1.f, 2.f, D3DCOLOR_XRGB(0, 255, 0));
	m_vtxList.emplace_back(1.f, 0.f, 2.f, D3DCOLOR_XRGB(0, 0, 255));
}

CTriangle::~CTriangle()
{
}
