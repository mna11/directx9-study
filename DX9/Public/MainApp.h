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
	ID3DXEffect* m_pToonEffect = {};

	ID3DXMesh* m_pMeshes[4] = { {}, {}, {}, {} };
	D3DXMATRIX m_matWorld[4] = { {}, {}, {}, {} };
	D3DXVECTOR4 m_vMeshColor[4] = { {}, {}, {}, {} };

	D3DXMATRIX m_matProj = {};
	D3DXMATRIX m_matView = {};
	IDirect3DTexture9* m_pShadeTex = {};

	D3DXHANDLE m_hToonWorldView = {};
	D3DXHANDLE m_hToonWorldViewProj = {};
	D3DXHANDLE m_hToonColor = {};
	D3DXHANDLE m_hToonLightDir = {};
	D3DXHANDLE m_hShadeTex = {};
	D3DXHANDLE m_hToonTech = {};
};
