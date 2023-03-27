#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc;
	PAINTSTRUCT ps;
	RECT        rect;

	switch(message)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);

	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{

	static TCHAR szAppName[] = TEXT("HelloWin");
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,                  // window class name
		TEXT("The Hello Program"), // window caption
		WS_OVERLAPPEDWINDOW,        // window style
		CW_USEDEFAULT,              // initial x position
		CW_USEDEFAULT,              // initial y position
		1024,              // initial x size
		768,              // initial y size
		NULL,                       // parent window handle
		NULL,                       // window menu handle
		hInstance,                  // program instance handle
		NULL);                     // creation parameters

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);



	// D3D
	IDirect3D9 *d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	//D3DPRESENT_PARAMETERS d3dpp;
	//memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	//d3dpp.BackBufferWidth = 1024;
	//d3dpp.BackBufferHeight = 768;
	//d3dpp.BackBufferFormat = D3DFMT_R8G8B8;
	//d3dpp.BackBufferCount = 1;
	//d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	//d3dpp.MultiSampleQuality = 0;
	//d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//d3dpp.EnableAutoDepthStencil = TRUE;
	//d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//d3dpp.Windowed = TRUE;


	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
	d3dpp.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	IDirect3DDevice9 *d3ddev9 = NULL;
	HRESULT r = d3d9->lpVtbl->CreateDevice(d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &d3ddev9);
	if(FAILED(r))
	{
		char msg[9999];
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)r, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)msg, 1000, NULL);
		printf("%s\n", msg);
		return 0;
	}

	d3ddev9->lpVtbl->Clear(d3ddev9, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(221, 0, 18), 1.0f, 0);
	d3ddev9->lpVtbl->BeginScene(d3ddev9);
	d3ddev9->lpVtbl->EndScene(d3ddev9);
	d3ddev9->lpVtbl->Present(d3ddev9, NULL, NULL, NULL, NULL);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	return msg.wParam;
}
