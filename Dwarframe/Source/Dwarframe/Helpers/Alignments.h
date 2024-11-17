#ifndef ALIGNMENTS_H
#define ALIGNMENTS_H

#include "Dwarframe/Core.h"

namespace Dwarframe {

	constexpr uint64 ConstantBufferAlignement = 256;
	constexpr uint64 TextureAlignement = MEM_KiB(64);

	template <typename T>
	inline constexpr T GetSizeAligned(T DataSize, T Alignement)
	{
		return (DataSize + (Alignement - 1)) & ~(Alignement - 1);
	}

	template <typename T>
	inline constexpr T GetSizeBufferAligned(T DataSize)
	{
		return (DataSize + ((T)D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~((T)D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
	}
}

#endif // !ALIGNMENTS_H