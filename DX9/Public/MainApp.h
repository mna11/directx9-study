#pragma once

class CCube;

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
	CCube* m_Cube;
	IDirect3DTexture9* m_texCube;
};

