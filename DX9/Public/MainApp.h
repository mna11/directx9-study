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
	vector<ID3DXMesh*> m_MeshObjList;
	vector<D3DXMATRIX> m_matWorldList;
	vector<D3DMATERIAL9> m_mtrlList;
};

