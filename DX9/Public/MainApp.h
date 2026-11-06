#pragma once

class CCamera;

class CMainApp
{
public:
	CMainApp() = default;
	~CMainApp();
public: 
	bool Setup();
	void Cleanup();
	void Update(float timeDelta);
	bool Display();
private:
	bool DrawBasicScene(bool bClear, float fScale);
private:
	ID3DXMesh* m_pMesh = {};
	vector<D3DMATERIAL9> m_vecMtrl = {};
	vector<IDirect3DTexture9*> m_pTex = {};

	ID3DXEffect* m_pLightTexEffect = {};

	D3DXHANDLE m_hWorldMatrix = {};
	D3DXHANDLE m_hViewMatrix = {};
	D3DXHANDLE m_hProjMatrix = {};
	D3DXHANDLE m_hTex = {};
	D3DXHANDLE m_hLightTexTech = {};

};

