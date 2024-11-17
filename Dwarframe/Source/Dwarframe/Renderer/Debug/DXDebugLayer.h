#ifndef DXDEBUGLAYER_H
#define DXDEBUGLAYER_H

#include "Dwarframe/Core.h"

namespace Dwarframe {

	class DXDebugLayer
	{
	public:
		void Initialize();
		void Shutdown();
	
	private:
	#ifdef _DEBUG
		ComPtr<ID3D12Debug3> m_D3D12DebugInterface;
		ComPtr<IDXGIDebug1> m_DXGIDebugInterface;
	#endif

	public:
		DXDebugLayer(const DXDebugLayer& Other) = delete; 
		DXDebugLayer& operator=(const DXDebugLayer& Other) = delete; 

		inline static DXDebugLayer& Get()
		{
			static DXDebugLayer Instance;

			return Instance;
		}

	private:
		DXDebugLayer() = default;
	};

}

#endif
