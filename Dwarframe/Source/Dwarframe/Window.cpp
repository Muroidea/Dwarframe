#include "pch.h"
#include "Window.h"

#ifdef WITH_EDITOR
#include "ImGUI/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
#endif

namespace Dwarframe {

    LRESULT Window::WindowProcStaticSetup(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        // When message is equal WM_NCCREATE then lParam contains pointer to CREATESTRUCT that has values passed in CreateWindowEx function
        if (Msg == WM_NCCREATE)
        {
            // Retrieve pointer to window class
            CREATESTRUCTW* CreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
            Window* PointerToWindow = static_cast<Window*>(CreateStruct->lpCreateParams);

            // Set UserData to store pointer to instance of our class
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(PointerToWindow));
            // Change WndProc from setup version to the normal
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WindowProcStatic));

            // Forward message to window class message handler
            return PointerToWindow->WindowProc(hWnd, Msg, wParam, lParam);
        }

        // If we get a message before the WM_NCCREATE message, handle it with default window proc
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    LRESULT Window::WindowProcStatic(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        // Retrieve pointer to window class
        Window* PointerToWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        // Forward message to window class handler
        return PointerToWindow->WindowProc(hWnd, Msg, wParam, lParam);
    }

	Window::Window()
        : m_InstanceHandle(), m_WindowHandle(nullptr), m_WindowClass(0), m_WindowRect(), m_MessageInfo()
	{
	}

	void Window::Initialize()
	{
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        m_InstanceHandle = GetModuleHandle(NULL);

        WNDCLASSEX WindowClass = { 0 };
        WindowClass.cbSize = sizeof(WNDCLASSEX);
        WindowClass.hInstance = m_InstanceHandle;
        WindowClass.style = CS_HREDRAW | CS_VREDRAW;
        WindowClass.lpfnWndProc = WindowProcStaticSetup;
        WindowClass.cbClsExtra = 0;
        WindowClass.cbWndExtra = 0;
        WindowClass.hIcon = NULL;
        WindowClass.hIconSm = NULL;
        WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        WindowClass.lpszMenuName = nullptr;
        WindowClass.lpszClassName = m_WindowInfo.WindowName.c_str();

        m_WindowClass = RegisterClassEx(&WindowClass);
        if (!m_WindowClass)
        {
            MessageBoxA(NULL, "Unable to register the window class.", "Error", MB_OK | MB_ICONERROR);
            throw std::exception();
        }

        DWORD Style = WS_OVERLAPPEDWINDOW;
        DWORD ExtendedStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        if (m_WindowInfo.Fullscreen)
        {
            Style = WS_POPUP;
            ExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

            m_WindowInfo.Width = GetSystemMetrics(SM_CXSCREEN);
            m_WindowInfo.Height = GetSystemMetrics(SM_CYSCREEN);
        }

        int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
        int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

        m_WindowRect.left = 0;
        m_WindowRect.top = 0;
        m_WindowRect.right = m_WindowInfo.Width;
        m_WindowRect.bottom = m_WindowInfo.Height;

        // Consider chosen styles into final sizes
        AdjustWindowRectEx(&m_WindowRect, Style, FALSE, ExtendedStyle);

        int AdjustedWidth = m_WindowRect.right - m_WindowRect.left;
        int AdjustedHeight = m_WindowRect.bottom - m_WindowRect.top;

        // Center the window within the screen. Clamp to 0, 0 for the top-left corner.
        int WindowX = std::max<int>(0, static_cast<int>((ScreenWidth - AdjustedWidth) * 0.5));
        int WindowY = std::max<int>(0, static_cast<int>(((ScreenHeight - AdjustedHeight) * 0.5)));
        
        m_WindowHandle = CreateWindowEx(0, m_WindowInfo.WindowName.c_str(), m_WindowInfo.WindowName.c_str(), Style,
            WindowX, WindowY, AdjustedWidth, AdjustedHeight, NULL, NULL, m_InstanceHandle, this);
        
        if (!m_WindowHandle)
        {
            MessageBoxA(NULL, "Unable to create window.", "Error", MB_OK | MB_ICONERROR);
            throw std::exception();
        }

        ShowWindow(m_WindowHandle, SW_SHOW);

        // TODO: Extend raw input handling
        RAWINPUTDEVICE RawInputDevices[2];

        RawInputDevices[0].usUsagePage = 0x0001;
        RawInputDevices[0].usUsage = 0x0002;
        RawInputDevices[0].dwFlags = 0;
        RawInputDevices[0].hwndTarget = m_WindowHandle;

        RawInputDevices[1].usUsagePage = 0x0001;
        RawInputDevices[1].usUsage = 0x0006;
        RawInputDevices[1].dwFlags = 0;
        RawInputDevices[1].hwndTarget = m_WindowHandle;

        if (RegisterRawInputDevices(RawInputDevices, 2, sizeof(RawInputDevices[0])) == FALSE)
        {
            MessageBoxA(NULL, "Unable to register raw input.", "Error", MB_OK | MB_ICONERROR);
            throw std::exception();
        }
	}

	void Window::Shutdown()
	{
        DestroyWindow(m_WindowHandle);
        m_WindowHandle = nullptr;

        UnregisterClassW((LPCWSTR)m_WindowClass, m_InstanceHandle);
        m_WindowClass = 0;
	}
    
    LRESULT Window::WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept
    {   
    #ifdef WITH_EDITOR
        if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
        {
            return true;
        }
    #endif

        switch (Msg)
        {
            case WM_INPUT:
            {
                UINT dwSize {};
                GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
                LPBYTE lpb = new BYTE[dwSize];
                if (lpb == NULL)
                {
                    return 0;
                }

                if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
                {
                    MessageBoxA(NULL, "Inproper raw input data size.", "Error", MB_OK | MB_ICONERROR);
                    return -1;
                }

                RAWINPUT* raw = (RAWINPUT*)lpb;

                if (raw->header.dwType == RIM_TYPEKEYBOARD)
                {
                    if (raw->data.keyboard.Flags == RI_KEY_MAKE)
                    {
                        if (raw->data.keyboard.VKey == VK_F11)
                        {
                            ToggleFullscreen();
                        }
                    }
                    else if (raw->data.keyboard.Flags & RI_KEY_BREAK)
                    {
                    }
                }
                else if (raw->header.dwType == RIM_TYPEMOUSE)
                {
                    if (raw->data.mouse.usFlags & MOUSE_MOVE_RELATIVE)
                    {
                    }
                    else if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
                    {
                    }

                    if (raw->data.mouse.usButtonFlags & (RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_2_DOWN | RI_MOUSE_BUTTON_3_DOWN | RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_5_DOWN))
                    {
                    }
                    else if (raw->data.mouse.usButtonFlags & (RI_MOUSE_BUTTON_1_UP | RI_MOUSE_BUTTON_2_UP | RI_MOUSE_BUTTON_3_UP | RI_MOUSE_BUTTON_4_UP | RI_MOUSE_BUTTON_5_UP))
                    {
                    }

                    if (raw->data.mouse.usButtonFlags & (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL))
                    {
                        
                    }
                }

                delete[] lpb;
                return 0;
            }
            case WM_CLOSE: // Window should close message
            {
                PostQuitMessage(0);
                return 0;
            }
            case WM_SIZE:
            {
                if (lParam && (m_WindowInfo.Width != LOWORD(lParam) || m_WindowInfo.Height != HIWORD(lParam)))
                {
                    m_WindowInfo.Width = LOWORD(lParam);
                    m_WindowInfo.Height = HIWORD(lParam); 

                    if (m_OnResizeFunction) m_OnResizeFunction(m_WindowInfo.Width, m_WindowInfo.Height);
                }

                break;
            }
        }

        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    bool Window::HandleEvents()
    {
        // WM_QUIT posted by PostQuitMessage is not associated with any window so we cannot set HWND in function below
        while (PeekMessageW(&m_MessageInfo, 0, 0, 0, PM_REMOVE))
        {
            
            if (m_MessageInfo.message == WM_QUIT)
            {
                return false;
            }

            TranslateMessage(&m_MessageInfo);
            DispatchMessage(&m_MessageInfo);
        }

        return true;
    }

    void Window::ToggleFullscreen()
    {
        m_WindowInfo.Fullscreen = !m_WindowInfo.Fullscreen;

        // Setting styles
        DWORD Style = WS_OVERLAPPEDWINDOW;
        DWORD ExtendedStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        if (m_WindowInfo.Fullscreen)
        {
            Style = WS_POPUP;
            ExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;;
        }

        SetWindowLong(m_WindowHandle, GWL_STYLE, Style);
        SetWindowLong(m_WindowHandle, GWL_EXSTYLE, ExtendedStyle);

        if (m_WindowInfo.Fullscreen)
        {
            HMONITOR Monitor = MonitorFromWindow(m_WindowHandle, MONITOR_DEFAULTTONEAREST);
            MONITORINFO MonitorInfo {};
            MonitorInfo.cbSize = sizeof(MonitorInfo);
            if (GetMonitorInfoW(Monitor, &MonitorInfo))
            {
                m_WindowInfo.Width = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;
                m_WindowInfo.Height = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;

                SetWindowPos(m_WindowHandle, nullptr, 
                    MonitorInfo.rcMonitor.left, 
                    MonitorInfo.rcMonitor.top, 
                    MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left, 
                    MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                    SWP_SHOWWINDOW
                );
            }
        }
        else
        {
            ShowWindow(m_WindowHandle, SW_MAXIMIZE);
        }
    }
}