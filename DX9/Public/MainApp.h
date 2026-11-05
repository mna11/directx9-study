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
	IDirect3DVertexShader9* m_pTransformShader = {};
	ID3DXConstantTable* m_pTransformConstantTable = {};

	ID3DXMesh* m_pTeapot = {};
	D3DXHANDLE m_hTransformViewProj = {};
	D3DXMATRIX m_matProj = {};
};

