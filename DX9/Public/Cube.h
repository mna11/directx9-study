#pragma once

class CCube
{
public:
	CCube(IDirect3DDevice9* device);
	~CCube();

public:
	bool Render(D3DXMATRIX* world, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);

private:
	IDirect3DDevice9* _device;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9* _ib;
};

