#pragma once

class CMainApp
{
public:
	CMainApp();
	~CMainApp();
public: 
	bool Setup();
	void Cleanup();
	void Update(float timeDelta);
	bool Display();

private: 
	void RenderScene();
	void RenderMirror();
private:
	// 버텍스 버퍼
	IDirect3DVertexBuffer9* m_VB;
	
	// 텍스처
	IDirect3DTexture9* m_texFloor;
	IDirect3DTexture9* m_texWall;
	IDirect3DTexture9* m_texMirror;
	
	// 머테리얼
	D3DMATERIAL9 m_mtrlFloor;
	D3DMATERIAL9 m_mtrlWall;
	D3DMATERIAL9 m_mtrlMirror;

	// 주전자
	ID3DXMesh* m_pTeapot;
	D3DXVECTOR3 m_vTeapotPos;
	D3DMATERIAL9 m_mtrlTeapot;
};

