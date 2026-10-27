#pragma once 

#include <d3dx9.h>

namespace Client
{
	static constexpr unsigned int g_iWinSizeX = 800;
	static constexpr unsigned int g_iWinSizeY = 600;

	struct Vertex
	{
		Vertex() : _x(0), _y(0), _z(0) {};
		Vertex(float x, float y, float z) : _x(x), _y(y), _z(z) {};
		float _x, _y, _z;
		static const DWORD FVF = D3DFVF_XYZ;
	};
}

using namespace Client;

extern IDirect3DDevice9* Device;

