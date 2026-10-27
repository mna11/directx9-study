#include "pch.h"
#include "Triangle.h"

CTriangle::CTriangle()
{
	// 버텍스 초기화
	m_vtxList.emplace_back(-1.f, 0.f, 0.f);
	m_vtxList.emplace_back(0.f, 1.f, 0.f);
	m_vtxList.emplace_back(1.f, 0.f, 0.f);

	// 인덱스 초기화
	m_idxList.emplace_back(0);
	m_idxList.emplace_back(1);
	m_idxList.emplace_back(2);
}

CTriangle::~CTriangle()
{
}
