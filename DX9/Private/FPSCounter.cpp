#include "pch.h"
#include "FPSCounter.h"

CFPSCounter::CFPSCounter()
{
	m_Font = new CD3DFont(L"Times New Roman", 24, 0);
	m_Font->InitDeviceObjects(Device);
	m_Font->RestoreDeviceObjects();

	m_FrameCnt = 0;
	m_timeElapsed = 0.f;
	m_fps = 0.f;
	m_strFPS = {};
}

CFPSCounter::~CFPSCounter()
{
	if (m_Font)
	{
		m_Font->InvalidateDeviceObjects();
		m_Font->DeleteDeviceObjects();
		delete m_Font;
	}
}

void CFPSCounter::Update(float timeDelta)
{
	++m_FrameCnt;
	m_timeElapsed += timeDelta;
	if (m_timeElapsed >= 1.f)
	{
		m_fps = (float)m_FrameCnt / m_timeElapsed;
		m_strFPS = to_wstring(m_fps);

		m_timeElapsed = 0.f;
		m_FrameCnt = 0;
	}
}

bool CFPSCounter::Render(D3DCOLOR color)
{
	if (m_Font)
	{
		m_Font->DrawText(20, 20, color, m_strFPS.c_str());
	}
	return true;
}
