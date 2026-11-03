#pragma once

class CCamera;
class CTerrain;
class CFPSCounter;

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
	bool DrawBasicScene(bool bClear, float fScale);	
	void KeyInput(float timeDelta);
private:
	CCamera*  m_pCamera;
	CTerrain* m_pTerrain;
	CFPSCounter* m_pFPS;
};

