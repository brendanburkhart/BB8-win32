#include "MainWindow.h"

#include <chrono>

#include "framework.h"

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    MainWindow* window = NULL;

    // retrieve window instance pointer via lpParam from CreateWindow
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* create = (CREATESTRUCT*)lParam;
        window = (MainWindow*)create->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        window->hwnd = hwnd;
    } else {
        window = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (window) {
        return window->HandleMessage(uMsg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

MainWindow::MainWindow()
    : hwnd(nullptr),
      is_fullscreen(false),
      saved_window_state(),
      factory(nullptr),
      render_target(nullptr),
      simulation(1.0, 9.0, 50, 0.7, 2e-4, Vector3(0.0, 0.0, 1.0)),
      imu(&simulation) {};

MainWindow::~MainWindow() {
    COMSafeRelease(&render_target);
    COMSafeRelease(&factory);
}

BOOL MainWindow::Create(
    PCWSTR window_name,
    DWORD window_style,
    int width,
    int height
) {
    WNDCLASS wc = { };

    wc.lpfnWndProc = MainWindow::WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ClassName();
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_SPHERE));

    RegisterClass(&wc);

    hwnd = CreateWindow(
        ClassName(), window_name, window_style, CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, 0, 0, GetModuleHandle(NULL), this
    );

    return (hwnd ? TRUE : FALSE);
}

HWND MainWindow::Window() const { return hwnd; }

PCWSTR MainWindow::ClassName() const { return L"BB8MainWindow"; }

HRESULT MainWindow::CreateGraphicsResources() {
    HRESULT hr = S_OK;

    if (render_target == NULL) {
        RECT rc;
        GetClientRect(hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd, size),
            &render_target);

        render_device = RenderDevice(size.width, size.height);
    }

    return hr;
}

void MainWindow::DiscardGraphicsResources() {
    COMSafeRelease(&render_target);
}

void MainWindow::OnPaint(UINT message, WPARAM wParam, LPARAM lParam) {
    Display();
}

void MainWindow::OnResize() {
    InvalidateRect(hwnd, NULL, FALSE);

    if (render_target == nullptr) {
        return;
    }

    RECT rc;
    GetClientRect(hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

    render_target->Resize(size);
    render_device = RenderDevice(size.width, size.height);
}

void MainWindow::OnKeyDown(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (wParam) {
    case VK_ESCAPE:
        // quit on escape
        PostMessage(hwnd, WM_CLOSE, NULL, NULL);
        break;
    case VK_F11:
        // toggle fullscreen mode
        Fullscreen(!is_fullscreen);
        OnResize();
        break;
    default:
        visualization.OnKeyDown(wParam, lParam);
        break;
    }

    DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
    {
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
        return FAILED(hr) ? -1 : 0;
    }
    case WM_PAINT:
    {
        OnPaint(message, wParam, lParam);
        return 0;
    }
    case WM_SIZE:
    {
        OnResize();
        return 0;
    }
    case WM_KEYDOWN:
    {
        OnKeyDown(message, wParam, lParam);
        return 0;
    }
    case WM_DESTROY:
    {
        DiscardGraphicsResources();
        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return TRUE;
}

void MainWindow::Fullscreen(bool fullscreen) {
    if (!is_fullscreen) {
        // save current window styles and rect
        saved_window_state.style = GetWindowLong(hwnd, GWL_STYLE);
        saved_window_state.ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
        GetWindowRect(hwnd, &saved_window_state.position);
    }

    RECT window_rect;

    if (fullscreen) {
        // set full screen style
        DWORD fullscreen_style = saved_window_state.style & ~(WS_CAPTION | WS_THICKFRAME);
        DWORD fullscreen_ex_style = saved_window_state.style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

        SetWindowLong(hwnd, GWL_STYLE, fullscreen_style);
        SetWindowLong(hwnd, GWL_EXSTYLE, fullscreen_ex_style);

        // set window to fill nearest monitor
        MONITORINFO monitor_info;
        monitor_info.cbSize = sizeof(monitor_info);
        BOOL success = GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitor_info);
        window_rect = monitor_info.rcMonitor;
    } else {
        // restore window styles
        SetWindowLong(hwnd, GWL_STYLE, saved_window_state.style);
        SetWindowLong(hwnd, GWL_EXSTYLE, saved_window_state.ex_style);

        // restore window rect
        window_rect = saved_window_state.position;
    }

    // apply desired window rect
    LONG width = window_rect.right - window_rect.left;
    LONG height = window_rect.bottom - window_rect.top;
    SetWindowPos(hwnd, NULL,
        window_rect.left, window_rect.top, width, height,
        SWP_NOZORDER | SWP_NOACTIVATE);

    is_fullscreen = fullscreen;
}

void MainWindow::Begin(int fps) {
    using clock = std::chrono::high_resolution_clock;
    using time = std::chrono::time_point<clock>;
    time current_time = clock::now(); // Current system time
    time next_render = current_time;    // Time to render next frame
    time last_render = current_time;    // System time at last update

    clock::duration frame_duration = std::chrono::duration_cast<clock::duration>(std::chrono::duration<double>(1.0 / fps));

    // Initialize the message structure
    MSG msg;
    PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

    // Run the message loop.
    while (msg.message != WM_QUIT) {
        // Is there a message to process?
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            // Dispatch the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            current_time = clock::now();
            if (current_time < next_render) { continue; }
            if (!SUCCEEDED(CreateGraphicsResources())) { continue; }

            auto elapsed_time = std::chrono::duration<double>(current_time - last_render);
            double dt = elapsed_time.count();
            visualization.Update(
                dt, simulation.get_position(), simulation.get_rotation(),
                simulation.get_platform_rotation(), simulation.get_pendulum_rotation(), simulation.get_heading());
            visualization.Render(render_device);
            Display();

            // Set time for next frame
            next_render += frame_duration;
            last_render = current_time;

            // If more than a frame behind, drop frame(s)
            if (next_render < current_time) {
                next_render = current_time + frame_duration;
            }

            simulation.update(dt);
        }
    }
}

void MainWindow::Display() {
    HRESULT hr = CreateGraphicsResources();

    if (!SUCCEEDED(hr)) {
        return;
    }

    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);

    render_target->BeginDraw();
    render_device.PresentTo(render_target);
    hr = render_target->EndDraw();

    if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
        DiscardGraphicsResources();
    }

    EndPaint(hwnd, &ps);
}
