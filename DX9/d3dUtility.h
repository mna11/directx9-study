#pragma once


namespace d3d
{
	// 메인 윈도우 초기화, D3D 초기화, 성공시 IDirect3DDevice9 포인터 리턴
	bool InitD3D(
		HINSTANCE hInstance,
		int width, int height,	//[입력] 후면 버퍼 크기
		bool windowed,			//[입력] 윈도우(true) or 전체화면(false)

		D3DDEVTYPE deviceType,	//[입력] 하드웨어(HAL) or 소프트웨어 (REF)
		IDirect3DDevice9** device
	);

	// 애플리케이션의 메시지 루프를 포장
	// 함수는 디스플레이 함수가 될 함수의 포인터를 받음
	int EnterMsgLoop(std::function<void(float)> ptr_update, std::function<bool()> ptr_display);

	// 메인 애플리케이션 윈도우를 위한 윈도우 프로시저
	LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);

	// COM 객체 해제
	template<class T> void Release(T t)
	{
		t->Release();
		t=0;
	}

	// 객체 제거
	template<class T> void Delete(T t)
	{
		if(t)
		{
			delete t;
			t=0;
		}
	}

	const D3DXCOLOR WHITE(D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR	BLACK(D3DCOLOR_XRGB(0, 0, 0));
	const D3DXCOLOR	RED(D3DCOLOR_XRGB(255, 0, 0));
	const D3DXCOLOR	GREEN(D3DCOLOR_XRGB(0, 255, 0));
	const D3DXCOLOR	BLUE(D3DCOLOR_XRGB(0, 0, 255));

	const D3DXCOLOR	YELLOW(D3DCOLOR_XRGB(255, 255, 0));
	const D3DXCOLOR	CYAN(D3DCOLOR_XRGB(0, 255, 255));
	const D3DXCOLOR	MAGENTA(D3DCOLOR_XRGB(255, 0, 255));
}