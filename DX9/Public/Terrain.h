#pragma once
class CTerrain
{
public:
	CTerrain(
		wstring heightmapFileName,
		int     numVertsPerRow,
		int     numVertsPerCol,
		int     cellSpacing,	// space between cells
		float   heightScale
	);
	~CTerrain();
public:
	int GetHeightMapEntry(int row, int col);
	void SetHeightMapEntry(int row, int col, int value);

	float GetHeight(float x, float z);

	bool LoadTexture(wstring fileName);
	bool GenTexture(D3DXVECTOR3* directionToLight);
	bool Render(D3DXMATRIX* world, bool drawTris);

private:
	IDirect3DTexture9* m_Tex{ nullptr };
	IDirect3DVertexBuffer9* m_VB{ nullptr };
	IDirect3DIndexBuffer9* m_IB{ nullptr };

	int m_NumVertsPerRow;
	int m_NumVertsPerCol;
	int m_CellSpacing;

	int m_NumCellsPerRow;
	int m_NumCellsPerCol;
	int m_Width;
	int m_Depth;
	int m_NumVertices;
	int m_NumTriangles;

	float m_HeightScale;

	vector<int> m_Heightmap;

private:
	bool ReadRawFile(wstring filename);
	bool ComputeVertices();
	bool ComputeIndices();
	bool LightTerrain(D3DXVECTOR3* directionToLight);
	float ComputeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight);

	struct TerrainVertex
	{
		TerrainVertex()
			: _x(0), _y(0), _z(0), _u(0), _v(0) {
		}
		TerrainVertex(float x, float y, float z, float u, float v)
			: _x(x), _y(y), _z(z), _u(u), _v(v) {
		}
		float _x, _y, _z;
		float _u, _v;

		static const DWORD FVF;
	};

};
