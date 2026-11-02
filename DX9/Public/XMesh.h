#pragma once
class CXMesh
{
public:
	CXMesh();
	~CXMesh();

public:
	void Render();

private:
	ID3DXMesh* m_Mesh{ 0 };
	vector<D3DMATERIAL9> m_Mtrl{};
	vector<IDirect3DTexture9*> m_Tex{};
};

