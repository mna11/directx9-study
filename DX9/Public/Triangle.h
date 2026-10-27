#pragma once
class CTriangle
{
public:
	CTriangle();
	~CTriangle();
public:
	const list<Vertex>& GetVtxList() const { return m_vtxList; }
	const list<WORD>& GetIdxList() const { return m_idxList; }
private:
	list<Vertex> m_vtxList;
	list<WORD>	 m_idxList;
};

