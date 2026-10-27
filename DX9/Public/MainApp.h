#pragma once

class CCube;
class CTeapot;
class CTriangle;

class CMainApp
{
public:
	enum RENDER_OBJ { CUBE, TEAPOT, TRIANGLE };

public:
	CMainApp();
	~CMainApp();
public: 
	bool Setup();
	void Cleanup();
	void Update(float timeDelta);
	bool Display();

private:
	IDirect3DVertexBuffer9* m_VB = 0;
	IDirect3DIndexBuffer9*	m_IB = 0;

	RENDER_OBJ	m_eRenderObj; 

	CCube*		m_pCube;
	CTeapot*	m_pTeapot;
	CTriangle*	m_pTriangle;
};

