#include "pch.h"
#include "Pyramid.h"

CPyramid::CPyramid()
{
	// 전면
	m_vtxList.emplace_back(-1.f, 0.f, -1.f, 0.f, 0.707f, -0.707f);
	m_vtxList.emplace_back(0.f, 1.f, 0.f, 0.f, 0.707f, -0.707f);
	m_vtxList.emplace_back(1.f, 0.f, -1.f, 0.f, 0.707f, -0.707f);

	// 왼쪽 측면
	m_vtxList.emplace_back(-1.f, 0.f, 1.f, -0.707f, 0.707f, 0.f);
	m_vtxList.emplace_back(0.f, 1.f, 0.f, -0.707f, 0.707f, 0.f);
	m_vtxList.emplace_back(-1.f, 0.f, -1.f, -0.707f, 0.707f, 0.f);

	// 오른쪽 측면
	m_vtxList.emplace_back(1.f, 0.f, -1.f, 0.707f, 0.707f, 0.f);
	m_vtxList.emplace_back(0.f, 1.f, 0.f, 0.707f, 0.707f, 0.f);
	m_vtxList.emplace_back(1.f, 0.f, 1.f, 0.707f, 0.707f, 0.f);

	// 후면
	m_vtxList.emplace_back(1.f, 0.f, 1.f, 0.f, 0.707f, 0.707f);
	m_vtxList.emplace_back(0.f, 1.f, 0.f, 0.f, 0.707f, 0.707f);
	m_vtxList.emplace_back(-1.f, 0.f, 1.f, 0.f, 0.707f, 0.707f);
}

CPyramid::~CPyramid()
{
}
