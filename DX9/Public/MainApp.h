#pragma once

class CD3DFont;

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
	CD3DFont* m_Font;
	DWORD m_FrameCnt;
	float m_TimeElapsed;
	float m_FPS;
	wstring m_strFPS;
};

