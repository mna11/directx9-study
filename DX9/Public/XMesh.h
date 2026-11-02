#pragma once
class CXMesh
{
public:
	CXMesh();
	~CXMesh();

public:
	void Render();

public:
	ID3DXPMesh* GetMesh() const { return m_PMesh; }

private:
	ID3DXPMesh* m_PMesh{ 0 };
	vector<D3DMATERIAL9> m_Mtrl{};
	vector<IDirect3DTexture9*> m_Tex{};
};

