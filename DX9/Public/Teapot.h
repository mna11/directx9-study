#pragma once
class CTeapot
{
public:
	CTeapot();
	~CTeapot();

public:
	ID3DXMesh* GetMesh() const { return m_Mesh; }

private:
	ID3DXMesh* m_Mesh;
};

