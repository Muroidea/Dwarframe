#ifndef BUFFER_H
#define BUFFER_H

#include "GPUResourceBase.h"

namespace Dwarframe {

	// Can be used to create ConstanBuffers, VertexBuffers, IndexBuffers
	class Buffer : public GPUResourceBase
	{
	public:
		Buffer(ID3D12Device* Device, size_t SizeInBytes, std::wstring DebugName = L"Buffer", D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_COPY_DEST);
		
	private:
	};

}

#endif // !BUFFER_H
