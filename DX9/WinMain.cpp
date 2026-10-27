
#include "pch.h"
#include "d3dUtility.h"
#include "MainApp.h"

IDirect3DDevice9* Device = 0;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int showCmd)
{
	if (!d3d::InitD3D(hInstance, g_iWinSizeX, g_iWinSizeY, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, L"InitD3D() - FAILED", 0, 0);
		return 0;
	}

	CMainApp MainApp;

	if (!MainApp.Setup())
	{
		::MessageBox(0, L"Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(
		[&MainApp](float timeDelta){
			MainApp.Update(timeDelta);
		},
		[&MainApp]()->bool {
			return MainApp.Display();
		}
	);

	MainApp.Cleanup();

	Device->Release();

	return 0;
}
