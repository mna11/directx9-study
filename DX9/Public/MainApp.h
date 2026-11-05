#pragma once

class CCamera;
class CSilhouetteEdge;

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
	IDirect3DVertexShader9* m_pToonShader = {};
	ID3DXConstantTable* m_pToonConstTable = {};

	ID3DXMesh* m_pMeshes[4] = { {}, {}, {}, {} };
	D3DXMATRIX m_matWorld[4] = { {}, {}, {}, {} };
	D3DXVECTOR4 m_vMeshColor[4] = { {}, {}, {}, {} };

	D3DXMATRIX m_matView = {};
	D3DXMATRIX m_matProj = {};
	IDirect3DTexture9* m_pShadeTex = {};

	D3DXHANDLE m_hToonWorldView = {};
	D3DXHANDLE m_hToonWorldViewProj = {};
	D3DXHANDLE m_hToonColor = {};
	D3DXHANDLE m_hToonLightDir = {};

	CSilhouetteEdge* m_pMeshOutline[4] = { {}, {}, {}, {} };
	IDirect3DVertexShader9* m_pOutlineShader = {};
	ID3DXConstantTable* m_pOutlineConstTable = {};

	D3DXHANDLE m_hOutlineWorldView = {};
	D3DXHANDLE m_hOutlineProjHandle = 0;
};

