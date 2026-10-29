#pragma once

class CQuad;
class CCube;

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
	IDirect3DTexture9* m_pTexQuad;

	CCube* m_pCube;
	IDirect3DTexture9* m_pTexCube;
	D3DXMATRIX m_matWorldCube;
};

