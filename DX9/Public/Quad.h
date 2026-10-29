#pragma once
class CQuad
{
private:
	CQuad();
	
public:
	~CQuad();
	void Initialize(IDirect3DDevice9* device);
	static CQuad* Create(IDirect3DDevice9* device);
	void Render(D3DXMATRIX* worldMatrix, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);

private:
	IDirect3DDevice9* m_device;
	IDirect3DVertexBuffer9* m_vb;
};

