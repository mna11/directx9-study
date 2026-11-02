#pragma once
class CMesh
{
public:
	CMesh();
	~CMesh();

public:
	ID3DXMesh* GetMesh() const { return m_Mesh; }
	const DWORD GetNumSubset() const { return m_NumSubSets; }

	HRESULT Render();

private:
	ID3DXMesh* m_Mesh = 0;
	const DWORD m_NumSubSets = 3;
	IDirect3DTexture9* m_Textures[3] = { 0, 0, 0 };
};

