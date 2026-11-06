#pragma once

class CCamera;
class CTerrain;

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
	void KeyInput(float timeDelta);
private:
	CTerrain* m_pTerrain = {};
	CCamera* m_pCamera = {};

	ID3DXEffect* m_pFogEffect = {};
	D3DXHANDLE m_hFogTech = {};
};

