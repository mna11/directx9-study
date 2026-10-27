#pragma once

#include <d3dx9.h>

namespace Client
{
	static constexpr unsigned long g_iWinSizeX = 1280;
	static constexpr unsigned long g_iWinSizeY = 720;

	struct ColorVertex
	{
		ColorVertex(float x, float y, float z, D3DCOLOR color) : _x(x), _y(y), _z(z), _color(color) {}
		float _x, _y, _z;
		D3DCOLOR _color;
		static const DWORD FVF;
	};
	const DWORD ColorVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
}

using namespace Client;
extern IDirect3DDevice9* Device;

