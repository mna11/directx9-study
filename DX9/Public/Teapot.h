#pragma once
class CTeapot
{
private:
	CTeapot();

public:
	~CTeapot();
	void Initialize(IDirect3DDevice9* device);
	static CTeapot* Create(IDirect3DDevice9* device);
	void Render(D3DXMATRIX* worldMat, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);
private:
	IDirect3DDevice9* m_device;
	ID3DXMesh*		  m_mesh;
};

