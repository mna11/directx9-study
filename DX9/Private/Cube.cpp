#include "pch.h"
#include "Cube.h"

CCube::CCube()
{
	// 버텍스 초기화
	m_vtxList.emplace_back(-1.f, -1.f, -1.f);
	m_vtxList.emplace_back(-1.f, 1.f, -1.f);
	m_vtxList.emplace_back(1.f, 1.f, -1.f);
	m_vtxList.emplace_back(1.f, -1.f, -1.f);
	m_vtxList.emplace_back(-1.f, -1.f, 1.f);
	m_vtxList.emplace_back(-1.f, 1.f, 1.f);
	m_vtxList.emplace_back(1.f, 1.f, 1.f);
	m_vtxList.emplace_back(1.f, -1.f, 1.f);


	// 인덱스 초기화

	// 전면 (z = -1)
	m_idxList.emplace_back(0);
	m_idxList.emplace_back(1);
	m_idxList.emplace_back(2);
	m_idxList.emplace_back(0);
	m_idxList.emplace_back(2);
	m_idxList.emplace_back(3);

	// 후면 (z = +1)
	m_idxList.emplace_back(4);
	m_idxList.emplace_back(6);
	m_idxList.emplace_back(5);
	m_idxList.emplace_back(4);
	m_idxList.emplace_back(7);
	m_idxList.emplace_back(6);

	// 왼쪽 측면 (x = -1)
	m_idxList.emplace_back(4);
	m_idxList.emplace_back(5);
	m_idxList.emplace_back(1);
	m_idxList.emplace_back(4);
	m_idxList.emplace_back(1);
	m_idxList.emplace_back(0);

	// 오른쪽 측면 (x = +1)
	m_idxList.emplace_back(3);
	m_idxList.emplace_back(2);
	m_idxList.emplace_back(6);
	m_idxList.emplace_back(3);
	m_idxList.emplace_back(6);
	m_idxList.emplace_back(7);

	// 상단 (y = +1)
	m_idxList.emplace_back(1);
	m_idxList.emplace_back(5);
	m_idxList.emplace_back(6);
	m_idxList.emplace_back(1);
	m_idxList.emplace_back(6);
	m_idxList.emplace_back(2);

	// 하단 (y = -1)
	m_idxList.emplace_back(4);
	m_idxList.emplace_back(0);
	m_idxList.emplace_back(3);
	m_idxList.emplace_back(4);
	m_idxList.emplace_back(3);
	m_idxList.emplace_back(7);
}

CCube::~CCube()
{
}
