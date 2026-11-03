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

	// D3DMATERIAL9 초기화
	D3DMATERIAL9 InitMtrl(
		D3DXCOLOR a,	// 환경광   Ambient
		D3DXCOLOR d,	// 난반사광 Diffuse
		D3DXCOLOR s,	// 정반사광 Specular
		D3DXCOLOR e,	// 방출광   Emssive
		float	  p		// 정반사광의 세기 Power 
	);

	//D3DLIGHT9 구조체를 이용해 광원을 지정한다.

	// 방향성 광원 초기화
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	// 점 광원 초기화
	D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
	// 스포트 광원 초기화
	D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);

	// 애플리케이션의 메시지 루프를 포장
	// 함수는 디스플레이 함수가 될 함수의 포인터를 받음
	int EnterMsgLoop(std::function<void(float)> ptr_update, std::function<bool()> ptr_display);

	float GetRandomFloat(float lowBound, float highBound);
	void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);
	DWORD FtoDw(float f);
	float Lerp(float a, float b, float t);

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
		t = 0;
	}

	// 객체 제거
	template<class T> void Delete(T t)
	{
		if (t)
		{
			delete t;
			t = 0;
		}
	}

	// 컬러
	const D3DXCOLOR WHITE(D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR	BLACK(D3DCOLOR_XRGB(0, 0, 0));
	const D3DXCOLOR	RED(D3DCOLOR_XRGB(255, 0, 0));
	const D3DXCOLOR	GREEN(D3DCOLOR_XRGB(0, 255, 0));
	const D3DXCOLOR	BLUE(D3DCOLOR_XRGB(0, 0, 255));

	const D3DXCOLOR	YELLOW(D3DCOLOR_XRGB(255, 255, 0));
	const D3DXCOLOR	CYAN(D3DCOLOR_XRGB(0, 255, 255));
	const D3DXCOLOR	MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

	const D3DXCOLOR BEACH_SAND(D3DCOLOR_XRGB(255, 249, 157));
	const D3DXCOLOR DESERT_SAND(D3DCOLOR_XRGB(250, 205, 135));

	const D3DXCOLOR LIGHTGREEN(D3DCOLOR_XRGB(60, 184, 120));
	const D3DXCOLOR  PUREGREEN(D3DCOLOR_XRGB(0, 166, 81));
	const D3DXCOLOR  DARKGREEN(D3DCOLOR_XRGB(0, 114, 54));

	const D3DXCOLOR LIGHT_YELLOW_GREEN(D3DCOLOR_XRGB(124, 197, 118));
	const D3DXCOLOR  PURE_YELLOW_GREEN(D3DCOLOR_XRGB(57, 181, 74));
	const D3DXCOLOR  DARK_YELLOW_GREEN(D3DCOLOR_XRGB(25, 123, 48));

	const D3DXCOLOR LIGHTBROWN(D3DCOLOR_XRGB(198, 156, 109));
	const D3DXCOLOR DARKBROWN(D3DCOLOR_XRGB(115, 100, 87));

	// 조명 - embient가 BLACK이다 : 빛을 발산하지 않는다.
	const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 8.f);
	const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 8.f);
	const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 8.f);
	const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 8.f);
	const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 8.f);

	// 상수
	const float D3DINFINITY = FLT_MAX;
	const float D3DEPSILON = 0.001f;
}