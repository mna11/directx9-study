#include "pch.h"
#include "d3dUtility.h"

bool d3d::InitD3D(HINSTANCE hInstance, int width, int height, bool windowed,
	D3DDEVTYPE deviceType, IDirect3DDevice9** device)
{
	/*================================================
		Window 생성
	  ================================================*/
	WNDCLASS WndClass;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)d3d::WndProc;
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = TEXT("Direct3DApp");
	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&WndClass);

	HWND hWnd = NULL;
	hWnd = ::CreateWindow(TEXT("Direct3DApp"), TEXT("d3dCubeApp"), WS_EX_TOPMOST,
		0, 0, width, height, NULL, (HMENU)NULL, hInstance, NULL);
	::ShowWindow(hWnd, SW_SHOW);
	::UpdateWindow(hWnd);

	/*================================================
		D3D 시작
	  ================================================*/

	  //1. Direct3D9의 객체 생성
	IDirect3D9* d3d9 = NULL;
	if (NULL == (d3d9 = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		::MessageBox(NULL, TEXT("Direct3D9 생성 실패"), NULL, MB_OK);
		return false;
	}
	//2. 하드웨어 버텍스 프로세싱 확인
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);
	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//3. D3DPRESENT_PARAMETERS 구조체 체우기.
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;	//깊이, 스텐실 버퍼의 포맷
	/*
		D3DFMT_D32		: 32비트 깊이 버퍼
		D3DFMT_D24S8	: 24비트 깊이 버퍼로 지정하고 스텐실 버퍼 8비트
		D3DFMT_D24X8	: 24비트 깊이 버퍼
		D3DFMT_D24X44S4	: 24비트 깊이 버퍼로 지정하고 스텐실 버퍼 4비트
		D3DFMT_D16		: 16비트 깊이 버퍼를 지정
	*/
	d3dpp.BackBufferCount = 1;					//이용할 후면 버퍼의 수
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;		//후면 버퍼의 픽셀 포멧
	d3dpp.BackBufferHeight = height;				//후면 버퍼의 높이
	d3dpp.BackBufferWidth = width;				//후면 버퍼의 너비
	d3dpp.EnableAutoDepthStencil = true;			//Direct3D가 자동으로 깊이/스텐실 버퍼를 관리하게 하려면 TRUE;
	d3dpp.Flags = 0;								//몇가지 부가 특성(없을경우 0)
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	//화면 재생율 지정
	d3dpp.hDeviceWindow = hWnd;					//서비스와 연결된 윈도우 핸들
	d3dpp.MultiSampleQuality = 0;					//멀티샘플링 레벨
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;	//후면 버퍼에 이용할 멀티 샘플링 타입
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	//D3DPRESENT 집합 멤버
	/*
		자세한건 SDK문서 참조.
		D3DPRESENT_INTERVAL_IMMEDIATE	: 즉시 시연한다.
		D3DPRESENT_INTERVAL_DEFAULT		: 적절한 시연 간격을 Direct3D가 결정(보통 재생율과 동일하게 지정)
	*/
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		//플리핑 체인버퍼가 교환 방법
	d3dpp.Windowed = windowed;					//전체화면이면 FALSE, 윈도우면 TRUE

	//4. IDirect3DDevice9 인스턴스 생성
	HRESULT hr = NULL;
	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,		//기본 아댑터
		deviceType,				//장치 아댑터
		hWnd,					//장치와 연결된 윈도우 핸들
		vp,						//버텍스 프로세싱 타입
		&d3dpp,					//D3DPRESENT_PARAMETERS객체
		device					//생성된 장치 리턴
	);
	if (FAILED(hr))
	{
		::MessageBox(NULL, TEXT("CreateDevice()- FAILED"), NULL, MB_OK);
		return false;
	}
	d3d9->Release();				//d3d9객체 해제

	return true;
}

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = a;
	mtrl.Diffuse = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power = p;

	return mtrl;
}

D3DLIGHT9 d3d::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = *color * 0.4f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_POINT;
	light.Ambient = *color * 0.6f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Position = *position;
	light.Range = 1000.0f;			// 빛이 영향을 줄 수 있는 최대 거리
	light.Attenuation0 = 1.0f;		// 상수 감소 - 이 경우에는 감쇠하지 않겠다.
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 d3d::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_SPOT;
	light.Ambient = *color * 0.0f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Position = *position;
	light.Direction = *direction;
	light.Range = 1000.0f;
	light.Falloff = 1.0f;			// 안쪽 원뿔과 바깥쪽 원뿔의 빛의 세기차이
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta = 0.4f;				// 안쪽 원뿔의 각도   - 라디안 
	light.Phi = 0.9f;				// 바깥쪽 원뿔의 각도 - 라디안

	return light;
}


/*================================================
	메시지 루프
  ================================================*/
int d3d::EnterMsgLoop(std::function<void(float)> ptr_update, std::function<bool()> ptr_display)
{
	MSG Message;
	ZeroMemory(&Message, sizeof(Message));
	static float lastTime = (float)timeGetTime();

	while (Message.message != WM_QUIT)
	{
		if (::PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;
			ptr_update(timeDelta);
			ptr_display();
			lastTime = currTime;
		}
	}

	return Message.wParam;
}

float d3d::GetRandomFloat(float lowBound, float highBound)
{
	if (lowBound >= highBound) // bad input
		return lowBound;

	// [0, 1] 사이의 랜덤 float
	float f = (rand() % 10000) * 0.0001f;

	return (f * (highBound - lowBound) + lowBound);
}

void d3d::GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD d3d::FtoDw(float f)
{
	return *((DWORD*)&f);
}

// 선형 보간
float d3d::Lerp(float a, float b, float t)
{
	return (a + (b - a) * t);
}


LRESULT CALLBACK d3d::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hWnd);
		break;
	}

	// 기본창 메세지 처리 함수 호출
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
