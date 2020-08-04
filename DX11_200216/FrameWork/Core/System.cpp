#include "Framework.h"
#include "System.h"
#include "Resource.h"
namespace
{
	System* MainSystem = nullptr;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
	return MainSystem->WndProc(hwnd, msg, wParam, lParam);
}

bool System::AppInit()
{
	if (!MyRegisterClass())
		return false;
	if (!InitInstance())
		return false;
	return true;
}


System::System(HINSTANCE hinstance, wstring AppName, bool Enable4xMsaa)
	: instance(hinstance), isAppPaused(false), isMinmized(false), AppName(AppName), isMaxMized(false),
	isResizing(false), isEnable4xMsaa(Enable4xMsaa), MsaaQulity(0), ClientWidth(1280), ClientHeight(720), hWnd(nullptr)
{
	MainSystem = this;
}


System::~System()
{
}

LRESULT System::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wparam) == WA_INACTIVE)
		{
			isAppPaused = true;
			Time.Stop();
		}
		else
		{
			isAppPaused = false;
			Time.Start();
		}
		return 0;
	case WM_SIZE:
		ClientWidth = LOWORD(lparam);
		ClientHeight = HIWORD(lparam);
		SizeChanged(wparam);
		return 0;

	case WM_ENTERSIZEMOVE:
		isAppPaused = true;
		isResizing = true;
		Time.Stop();
		return 0;
	case WM_EXITSIZEMOVE:
		isAppPaused = false;
		isResizing = false;
		Time.Start();
		OnResize();
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lparam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lparam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_INPUT:

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		Mouse::Instance()->ProcessMsg(msg, wparam, lparam);
		return 0;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::Instance()->ProcessMsg(msg, wparam, lparam);
		return 0;

	case WM_ACTIVATEAPP:
		Mouse::Instance()->ProcessMsg(msg, wparam, lparam);
		Keyboard::Instance()->ProcessMsg(msg, wparam, lparam);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int System::Run()
{
	return 0;
}

ATOM System::MyRegisterClass()
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_DX11200216));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DX11200216);
	wcex.lpszClassName = AppName.c_str();
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL System::InitInstance()
{
	RECT R = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	hWnd = CreateWindowW(AppName.c_str(), AppName.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return TRUE;
}
