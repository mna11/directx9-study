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
public:
	IDirect3DVertexBuffer9* m_VB;
};

