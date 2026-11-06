#pragma once

class CCamera;

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
private:
	IDirect3DPixelShader9* m_pMultiTexPS = {};
	ID3DXConstantTable* m_pMultiTexCT = {};

	IDirect3DVertexBuffer9* m_pQuadVB = {};
	IDirect3DTexture9* m_pBaseTex = {};
	IDirect3DTexture9* m_pSpotLightTex = {};
	IDirect3DTexture9* m_pStringTex = {};

	D3DXHANDLE m_hBaseTex = {};
	D3DXHANDLE m_hSpotLightTex = {};
	D3DXHANDLE m_hStringTex = {};

	D3DXCONSTANT_DESC m_descBaseTex = {};
	D3DXCONSTANT_DESC m_descSpotLightTex = {};
	D3DXCONSTANT_DESC m_descStringTex = {};
};

