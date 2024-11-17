#ifndef WINDOW_H
#define WINDOW_H

#define NOMINMAX
#include <windows.h>
#include <string>
#include <functional>

namespace Dwarframe {

	class Window
	{
	public:
		void Initialize();
		void Shutdown();
	
		bool HandleEvents();
		void ToggleFullscreen();

		inline HWND GetWindowHandle() const { return m_WindowHandle; }

		inline uint32_t GetWindowWidth() const { return m_WindowInfo.Width; }
		inline uint32_t GetWindowHeight() const { return m_WindowInfo.Height; }

		static constexpr uint32_t GetNumOfFrames() { return s_NumOfFrames; }
		
	private:
		static LRESULT CALLBACK WindowProcStaticSetup(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK WindowProcStatic(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept;
		
		LRESULT WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) noexcept;

	public:
		struct WindowInfo
		{
			// Window resolution
			uint32_t Width = 1980;
			uint32_t Height = 1080;
			
			// Window state
			bool Fullscreen = false;
			bool VSync = true;

			// App Data
			std::wstring WindowName = L"Dwarframe";
		} m_WindowInfo;
		
		std::function<void(unsigned int, unsigned int)> m_OnResizeFunction;

	private:
		static constexpr size_t s_NumOfFrames = 2;

		HINSTANCE m_InstanceHandle;
		HWND m_WindowHandle;
		ATOM m_WindowClass;
		RECT m_WindowRect;

		// Message handling staff
		MSG m_MessageInfo;
		
		
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