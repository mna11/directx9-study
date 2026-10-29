#pragma once
class CQuad
{
public:
	CQuad();
	~CQuad();

public:
	const list<Vertex>& GetVtxList() const { return m_vtxList; }
	IDirect3DTexture9* GetTex() const { return m_tex; }
private:
	list<Vertex> m_vtxList;
	IDirect3DTexture9* m_tex;
};

