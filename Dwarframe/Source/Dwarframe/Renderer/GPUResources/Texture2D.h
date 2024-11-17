#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "GPUResourceBase.h"
#include "Dwarframe/Resources/Texture.h"

namespace Dwarframe {

	class Texture2D : public GPUResourceBase
	{
	public:
		Texture2D(ID3D12Device* Device, const Texture* TextureData, std::wstring DebugName = L"Buffer");
		
	private:
	};

}

#endif // !TEXTURE2D_H
