#ifndef GRAPHICSBRIDGE_H
#define GRAPHICSBRIDGE_H

#include "Dwarframe/Window.h"

namespace Dwarframe {

	inline const char* ShaderModelToShortcut(D3D_SHADER_MODEL ShaderModel)
	{
		switch (ShaderModel)
        {
			case D3D_SHADER_MODEL_6_0: return "6_0";
			case D3D_SHADER_MODEL_6_1: return "6_1";
			case D3D_SHADER_MODEL_6_2: return "6_2";
			case D3D_SHADER_MODEL_6_3: return "6_3";
			case D3D_SHADER_MODEL_6_4: return "6_4";
			case D3D_SHADER_MODEL_6_5: return "6_5";
			case D3D_SHADER_MODEL_6_6: return "6_6";
			case D3D_SHADER_MODEL_6_7: return "6_7";
			case D3D_SHADER_MODEL_6_8: return "6_8";
			case D3D_SHADER_MODEL_6_9: return "6_9";
			default: return "";
		}
	}

	class GraphicsBridge
	{
	public:
		void Initialize();
		void Shutdown();

		void InitializeSwapChain(HWND WindowHandle, unsigned int Width, unsigned int Height);
		void ResizeSwapChainsBuffers(unsigned int Width, unsigned int Height);
		void InitializeDepthStencilBuffer(unsigned int Width, unsigned int Height);

		void GetFrameBuffers();
		void ReleaseFrameBuffers();
		
		void BeginFrame(ID3D12GraphicsCommandList6* CommandList);
		void EndFrame(ID3D12GraphicsCommandList6* CommandList);

		inline void PresentAndSwapBuffers()
		{
			m_DXGISwapChain->Present(1, 0);
		}
		
		inline D3D_SHADER_MODEL GetShaderModel() const { return m_ShaderModel; }
		inline const char* GetShaderModelShortcut() const { return m_ShaderModelShortcut; }

		inline D3D12_CPU_DESCRIPTOR_HANDLE* GetCurrentRenderTarget() { return &m_RTVHandles[m_CurrentFrameBufferIndex]; }
		inline D3D12_CPU_DESCRIPTOR_HANDLE* GetDepthStencil() { return &m_DSVHandle; }
		
		inline IDXGIFactory7* GetDXGIFactory() { return m_DXGIFactory.Get(); }
		inline ID3D12Device8* GetD3D12Device() { return m_D3D12Device.Get(); }
		inline ID3D12CommandQueue* GetD3D12CommandQueue() { return m_D3D12CommandQueue.Get(); }

	private:
		//****** Supported features ******
		D3D_SHADER_MODEL m_ShaderModel;
		const char* m_ShaderModelShortcut;

		//****** DXGI basic data ******
		ComPtr<IDXGIFactory7> m_DXGIFactory;
		
		// Create manager for devices discovered by DXGI
		//std::vector<ComPtr<IDXGIOutput6>> m_DXGIAvailableDisplays;
		//std::vector<ComPtr<IDXGIAdapter4>> m_DXGIAvailableAdapters;
		//ComPtr<IDXGIOutput6> m_DXGIChosenDisplay;

		ComPtr<IDXGIAdapter4> m_DXGIChosenAdapter;

		ComPtr<IDXGISwapChain4> m_DXGISwapChain;

		//****** D3D12 basic data ******
		ComPtr<ID3D12Device8> m_D3D12Device;
		ComPtr<ID3D12CommandQueue> m_D3D12CommandQueue;
		
		//****** Frames render targets data ******
		ComPtr<ID3D12DescriptorHeap> m_D3D12RTVDescriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandles[Window::GetNumOfFrames()];
		ComPtr<ID3D12Resource> m_FrameBuffers[Window::GetNumOfFrames()];
		size_t m_CurrentFrameBufferIndex = 0;

		//****** Depth stencil data ******
		ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle;
		ComPtr<ID3D12Resource> m_DepthStencilBuffer;

	public:
		GraphicsBridge(const GraphicsBridge& Other) = delete; 
		GraphicsBridge& operator=(const GraphicsBridge& Other) = delete; 

		inline static GraphicsBridge& Get()
		{
			static GraphicsBridge Instance;

			return Instance;
		}

	private:
		GraphicsBridge() = default;
	};

}

#endif // !GRAPHICSBRIDGE_H
