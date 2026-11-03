#pragma once
class CBoundingVolume
{
public:
	enum BOUND_TYPE { BOX, SPHERE, END};
private:
	CBoundingVolume() = default;
public:
	~CBoundingVolume();

public:
	D3DXVECTOR3& GetMin() { return m_vMin; }
	D3DXVECTOR3& GetMax() { return m_vMax; }
	void SetMin(D3DXVECTOR3 vMin) { m_vMin = vMin; }
	void SetMax(D3DXVECTOR3 vMax) { m_vMax = vMax; }
	
	void SetBoundType(BOUND_TYPE eType) { m_eType = eType; }
	bool isPointInside(D3DXVECTOR3& p);
	bool ComputeBoundingSphere(ID3DXMesh* mesh);
	bool ComputeBoundingBox(ID3DXMesh* mesh);

	void Render();

public:
	static CBoundingVolume* Create(BOUND_TYPE eType);


private:
	BOUND_TYPE m_eType = BOX;
	
	// BOX
	D3DXVECTOR3 m_vMin = {d3d::D3DINFINITY, d3d::D3DINFINITY, d3d::D3DINFINITY};
	D3DXVECTOR3 m_vMax = {-d3d::D3DINFINITY, -d3d::D3DINFINITY, -d3d::D3DINFINITY };
	ID3DXMesh* m_BoxMesh{ 0 };

	// SPHERE
	D3DXVECTOR3 m_vCenter = { 0.f, 0.f, 0.f };
	float		m_fRadius = 0.f;
	ID3DXMesh* m_SphereMesh{ 0 };
};

