#pragma once
class CCube
{
private:
	CCube();

public:
	~CCube();
	void Initialize(IDirect3DDevice9* device);
	static CCube* Create(IDirect3DDevice9* device);
	void Render(D3DXMATRIX* matWorld, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);

private:
	IDirect3DDevice9* m_device;
	IDirect3DVertexBuffer9* m_vb;
	IDirect3DIndexBuffer9* m_ib;
};

