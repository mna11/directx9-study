#pragma once

class CMesh;

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
	void dumpVertices(ofstream& outFile, ID3DXMesh* mesh);
	void dumpIndices(ofstream& outFile, ID3DXMesh* mesh);
	void dumpAttributeBuffer(ofstream& outFile, ID3DXMesh* mesh);
	void dumpAdjacencyBuffer(std::ofstream& outFile, ID3DXMesh* mesh);
	void dumpAttributeTable(std::ofstream& outFile, ID3DXMesh* mesh);

private:
	CMesh* m_Mesh;
	ofstream m_OutFile;
};

