#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "ResourceManager.h"
#include "Dwarframe/Resources/Texture.h"

namespace Dwarframe {

	class Buffer;
	class UploadBuffer;

	class TextureManager : public ResourceManager<Texture>
	{
	public:
#if WITH_EDITOR
		bool Extends(std::string_view ElementName) override;

		virtual void RenderOptions() override;
		virtual void RenderResourceList(std::vector<ContentBasicInfo>& ResourceList) override;
#endif

		bool AddResource(std::filesystem::path AssetPath, std::string AssetName) override;
		
		void FillTransitionBarrierToNewState(D3D12_RESOURCE_BARRIER* Barriers, D3D12_RESOURCE_STATES NewState);
		void SubmitTextureDataToUpload(ID3D12Device8* Device, UploadBuffer* InUploadBuffer);
		void BindTexture(ID3D12GraphicsCommandList6* CommandList);

	public:
		TextureManager();
		TextureManager(const TextureManager& Other) = delete;
		TextureManager& operator=(const TextureManager& Other) = delete;
		virtual ~TextureManager() = default;
	};

}

#endif //!TEXTUREMANAGER_H