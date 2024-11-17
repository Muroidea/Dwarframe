#ifndef TEXTURE_H
#define TEXTURE_H

#include "GameAsset.h"

namespace Dwarframe {

    class Texture : public GameAsset
    {
	IMPLEMENT_SIMPLE_RTTI(Texture)

    public:
        Texture(std::string AssetPath, std::string AssetName);

        virtual bool LoadAsset() override;
        virtual bool SaveAsset() override;
        
#ifdef WITH_EDITOR
		virtual void RenderOptions() override;
#endif

        inline uint32_t GetWidth() const { return m_Width; }
        inline uint32_t GetHeight() const { return m_Height; }
        inline uint32_t GetDepth() const { return m_Depth; }
        inline uint32_t GetBitsPerPixel() const { return m_BitsPerPixel; }
        inline uint32_t GetChannelCount() const { return m_ChannelCount; }

        inline uint32_t GetStride() const { return m_Stride; }
        inline uint32_t GetSizeInBytes() const { return m_SizeInBytes; }

        inline void* GetData() const { return m_Data; }
        inline DXGI_FORMAT GetPixelFormat() const { return m_PixelFormat; }

    private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_Depth = 0;
		uint32_t m_BitsPerPixel = 0;
		uint32_t m_ChannelCount = 0;

		uint32_t m_Stride = 0;
		uint32_t m_SizeInBytes = 0;

        void* m_Data = nullptr;

        // DXGI should be changed to other internal format
        DXGI_FORMAT m_PixelFormat = DXGI_FORMAT_UNKNOWN;
        
        friend class TextureLoader;
        friend class TextureManager;
    };

}

#endif // !MESH_H