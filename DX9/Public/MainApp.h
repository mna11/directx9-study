#pragma once

class CBoundingVolume;

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
	Ray  CalcPickingRay(int x, int y);
	void TransformRay(Ray* ray, D3DXMATRIX* T);
	bool RaySphereIntTest(Ray* ray, CBoundingVolume* sphere);
private:
	D3DXMATRIX m_matWorld;
	CBoundingVolume* m_pBSphere;

	ID3DXMesh* m_pTeapot;
};

