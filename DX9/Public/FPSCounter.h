#pragma once
class CFPSCounter
{
public:
	CFPSCounter();
	~CFPSCounter();

	void Update(float timeDelta);
	bool Render(D3DCOLOR color);

private:
	CD3DFont* m_Font;
	DWORD     m_FrameCnt;
	float     m_timeElapsed;
	float     m_fps;
	wstring   m_strFPS;
};

