#pragma once
class CPyramid
{
public:
	CPyramid();
	~CPyramid();
public:
	const list<Vertex>& GetVtxList() const { return m_vtxList; }
private:
	list<Vertex> m_vtxList;
};

