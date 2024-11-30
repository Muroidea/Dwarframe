#ifndef WINDOW_H
#define WINDOW_H

#define NOMINMAX
#include <windows.h>
#include <string>
#include <functional>

#include "Dwarframe/Core/WindowsKeyMapper.h"

namespace Dwarframe {

	class Window
	{
	public:
		void Initialize();
		void Shutdown();
	
		bool HandleEvents();
		void ToggleFullscreen();

		inline HWND GetWindowHandle() const { return m_WindowHandle; }

		inline uint32 GetWindowWidth() const { return m_WindowInfo.Width; }
		inline uint32 GetWindowHeight() const { return m_WindowInfo.Height; }

		static constexpr uint32 GetNumOfFrames() { return s_NumOfFrames; }
		
	private:
		static LRESULT CALLBACK WindowProcStaticSetup(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK WindowProcStatic(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept;
		
		LRESULT WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept;

	public:
		struct WindowInfo
		{
			// Window resolution
			uint32 Width = 1980;
			uint32 Height = 1080;
			
			// Window state
			bool Fullscreen = false;
			bool VSync = true;

			// App Data
			std::wstring WindowName = L"Dwarframe";
		} m_WindowInfo;
		
		std::function<void(uint32, uint32)> m_OnResizeFunction;

	private:
		static constexpr uint64 s_NumOfFrames = 2;

		HINSTANCE m_InstanceHandle;
		HWND m_WindowHandle;
		ATOM m_WindowClass;
		RECT m_WindowRect;

		// Message handling staff
		MSG m_MessageInfo;
		WindowsKeyMapper m_KeyMapper;
		
	public:
		Window(const Window& Other) = delete; 
		Window& operator=(const Window& Other) = delete; 

		inline static Window& Get()
		{
			static Window Instance;

			return Instance;
		}

	private:
		Window();
	};

}

#endif // !WINDOW_H