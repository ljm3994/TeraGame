// DX11_200216.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "Framework.h"
#include "DX11_200216.h"
#include "FrameWork/Core/Application.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    //메모리 누수 경고 디버그 옵션 추가
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Application app(hInstance, WindowName);

    if (!app.Init())
        return 0;

    return app.Run();
}