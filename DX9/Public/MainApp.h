#pragma once

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
	IDirect3DVertexBuffer9* m_VB = 0;
	IDirect3DIndexBuffer9*	m_IB = 0;
};

