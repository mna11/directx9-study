#pragma once

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
	ID3DXFont* m_Font;

	DWORD	m_FrameCnt;
	float	m_TimeElapsed;
	float	m_FPS;
	wstring m_strFPS;
};

