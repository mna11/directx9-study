#pragma once
class CTriangle
{
public:
	CTriangle();
	~CTriangle();

public:
	const list<ColorVertex>& GetVtxList() const { return m_vtxList; }

private:
	list<ColorVertex>	m_vtxList;
};

