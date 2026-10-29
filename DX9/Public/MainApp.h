#pragma once

class CQuad;
class CTeapot;

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
	CQuad* m_pQuad;
	D3DMATERIAL9 m_mtrlQuad;
	IDirect3DTexture9* m_pTexQuad;
	D3DXMATRIX m_matWorldQuad;


	CTeapot* m_pTeapot;
	D3DMATERIAL9 m_mtrlTeapot;
	D3DXMATRIX m_matWorldTeapot;
};

