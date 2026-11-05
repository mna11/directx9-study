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
	IDirect3DVertexShader9* m_pDiffuseShader = {};
	ID3DXConstantTable* m_pDiffuseConstTable = {};

	ID3DXMesh* m_pTeapot = {};

	D3DXHANDLE m_hViewMatrix = {};
	D3DXHANDLE m_hViewProjMatrix = {};
	D3DXHANDLE m_hAmbientMtrl = {};
	D3DXHANDLE m_hDiffuseMtrl = {};
	D3DXHANDLE m_hLightDir = {};

	D3DXMATRIX m_matProj = {};
};

