#pragma once

#include <vector>

#include "framework.h"
#include "Resource.h"

#include "IMU.h"
#include "RenderDevice.h"
#include "Visualization.h"
#include "Simulation.h"

class MainWindow {
private:
    class WindowState {
    public:
        DWORD style;
        DWORD ex_style;
        RECT position;
    };

    WindowState saved_window_state;
    bool is_fullscreen;

    ID2D1Factory* factory;
    ID2D1HwndRenderTarget* render_target;

    RenderDevice render_device;

    Simulation simulation;
    IMU imu;
    Visualization visualization;

    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    OnPaint(UINT message, WPARAM wParam, LPARAM lParam);
    void    OnResize();
    void    OnKeyDown(UINT message, WPARAM wParam, LPARAM lParam);
    void    Display();

public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    MainWindow();
    ~MainWindow();

    BOOL Create(
        PCWSTR window_name,
        DWORD window_style,
        int width = 650,
        int height = 500
    );

    HWND Window() const;
    PCWSTR ClassName() const;

    void Fullscreen(bool fullscreen);

    void Begin(int fps);

protected:
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND hwnd;
};
