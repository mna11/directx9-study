#pragma once

#include <d3dx9.h>

namespace Client
{
	static constexpr unsigned long g_iWinSizeX = 1280;
	static constexpr unsigned long g_iWinSizeY = 720;

	struct Vertex
	{
		Vertex() : _x(0), _y(0), _z(0), _nx(0), _ny(0), _nz(0) {}
		Vertex(float x, float y, float z, float nx, float ny, float nz) : _x(x), _y(y), _z(z), _nx(nx), _ny(ny), _nz(nz) {}

		float _x, _y, _z; 
		float _nx, _ny, _nz;
		static const DWORD FVF;
	};
	const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

	struct ColorVertex
	{
		ColorVertex() : _x(0), _y(0), _z(0), _color(D3DCOLOR_XRGB(255, 255, 255)) {}
		ColorVertex(float x, float y, float z, D3DCOLOR color) : _x(x), _y(y), _z(z), _color(color) {}

		float _x, _y, _z;
		D3DCOLOR _color;
		static const DWORD FVF;
	};
	const DWORD ColorVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
}

using namespace Client;
extern IDirect3DDevice9* Device;

