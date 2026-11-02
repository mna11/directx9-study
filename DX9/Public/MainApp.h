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
	void KeyInput(float timeDelta);
private:
	CCamera* m_pCamera{nullptr};
};

