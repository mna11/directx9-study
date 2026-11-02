#include "pch.h"
#include "MainApp.h"
#include "Mesh.h"

CMainApp::~CMainApp()
{
	Cleanup();
}

bool CMainApp::Setup() // Initialize의 역할
{
	m_Mesh = new CMesh;

	// Dump Mesh Data to File
	m_OutFile.open("./Bin/Mesh_Dump.txt");
	dumpVertices(m_OutFile, m_Mesh->GetMesh());
	dumpIndices(m_OutFile, m_Mesh->GetMesh());
	dumpAttributeTable(m_OutFile, m_Mesh->GetMesh());
	dumpAttributeBuffer(m_OutFile, m_Mesh->GetMesh());
	dumpAdjacencyBuffer(m_OutFile, m_Mesh->GetMesh());
	m_OutFile.close();

	// 텍스처 설정
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// 빛 끄기
	Device->SetRenderState(D3DRS_LIGHTING, false);

	// 뷰 행렬
	D3DXVECTOR3 pos(0.f, 0.f, -4.f);
	D3DXVECTOR3 target(0.f, 0.f, 0.f);
	D3DXVECTOR3 up(0.f, 1.f, 0.f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	// 투영 행렬
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float) g_iWinSizeX / (float) g_iWinSizeY,
		1.f, 
		1000.f
	);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void CMainApp::Cleanup() // Release의 역할
{
	delete m_Mesh;
}

void CMainApp::Update(float timeDelta)
{
	D3DXMATRIX xRot;
	D3DXMatrixRotationX(&xRot, D3DX_PI * 0.2f);

	static float y = 0.f;
	D3DXMATRIX yRot;
	D3DXMatrixRotationY(&yRot, y);
	y += timeDelta;
	if (y >= 2 * D3DX_PI)
		y = 0.f;

	D3DXMATRIX world = xRot * yRot;
	Device->SetTransform(D3DTS_WORLD, &world);
}

bool CMainApp::Display() // Render의 역할
{
	if (Device)
	{
		// 초기화
		// 전체 화면을 지우겠다 (0, 0)
		// Target Surface와 깊이 버퍼를 지우겠다.
		// 검정색, z 값은 1로 (가장 멀게), 스텐실은 0으로
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0000000, 1.f, 0);
		Device->BeginScene();

		m_Mesh->Render();

		Device->EndScene();
		Device->Present(0, 0, 0, 0); // 후면 버퍼 시연
	}
	return true;
}

void CMainApp::dumpVertices(ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Vertices:" << endl;
	outFile << "---------\n" << endl;

	Vertex* v = 0; 
	m_Mesh->GetMesh()->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < mesh->GetNumVertices(); i++)
	{
		outFile << "Vertex " << i << ": (";
		outFile << v[i]._x << ", " << v[i]._y << ", " << v[i]._z << ", ";
		outFile << v[i]._nx << ", " << v[i]._ny << ", " << v[i]._nz << ", ";
		outFile << v[i]._u << ", " << v[i]._v << ")" << endl;
	}
	mesh->UnlockVertexBuffer();

	outFile << endl << endl;

}

void CMainApp::dumpIndices(ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Indices:" << std::endl;
	outFile << "--------" << std::endl << std::endl;

	WORD* indices = 0;
	mesh->LockIndexBuffer(0, (void**)&indices);

	for (int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle " << i << ": ";
		outFile << indices[i * 3] << " ";
		outFile << indices[i * 3 + 1] << " ";
		outFile << indices[i * 3 + 2] << std::endl;
	}
	mesh->UnlockIndexBuffer();

	outFile << std::endl << std::endl;
}

void CMainApp::dumpAttributeBuffer(ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Attribute Buffer:" << std::endl;
	outFile << "-----------------" << std::endl << std::endl;

	DWORD* attributeBuffer = 0;
	mesh->LockAttributeBuffer(0, &attributeBuffer);

	// an attribute for each face
	for (int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle lives in subset " << i << ": ";
		outFile << attributeBuffer[i] << std::endl;
	}
	mesh->UnlockAttributeBuffer();

	outFile << std::endl << std::endl;
}

void CMainApp::dumpAdjacencyBuffer(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Adjacency Buffer:" << std::endl;
	outFile << "-----------------" << std::endl << std::endl;

	// three enttries per face
	std::vector<DWORD> adjacencyBuffer(mesh->GetNumFaces() * 3);

	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);

	for (int i = 0; i < mesh->GetNumFaces(); i++)
	{
		outFile << "Triangle's adjacent to triangle " << i << ": ";
		outFile << adjacencyBuffer[i * 3] << " ";
		outFile << adjacencyBuffer[i * 3 + 1] << " ";
		outFile << adjacencyBuffer[i * 3 + 2] << std::endl;
	}

	outFile << std::endl << std::endl;
}

void CMainApp::dumpAttributeTable(std::ofstream& outFile, ID3DXMesh* mesh)
{
	outFile << "Attribute Table:" << std::endl;
	outFile << "----------------" << std::endl << std::endl;

	// number of entries in the attribute table
	DWORD numEntries = 0;

	mesh->GetAttributeTable(0, &numEntries);

	std::vector<D3DXATTRIBUTERANGE> table(numEntries);

	mesh->GetAttributeTable(&table[0], &numEntries);

	for (int i = 0; i < numEntries; i++)
	{
		outFile << "Entry " << i << std::endl;
		outFile << "-----------" << std::endl;

		outFile << "Subset ID:    " << table[i].AttribId << std::endl;
		outFile << "Face Start:   " << table[i].FaceStart << std::endl;
		outFile << "Face Count:   " << table[i].FaceCount << std::endl;
		outFile << "Vertex Start: " << table[i].VertexStart << std::endl;
		outFile << "Vertex Count: " << table[i].VertexCount << std::endl;
		outFile << std::endl;
	}

	outFile << std::endl << std::endl;
}
