// BB8.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "BB8.h"
#include "MainWindow.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    MainWindow window;

    if (!window.Create(L"BB8 simulator", WS_OVERLAPPEDWINDOW)) {
        return 0;
    }

    ShowWindow(window.Window(), nCmdShow);
    UpdateWindow(window.Window());

    window.Begin(60);

    return 0;
}
