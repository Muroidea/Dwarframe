#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

namespace Dwarframe {

	class Texture;

	class TextureLoader
	{
	public:
		static bool Load(const std::filesystem::path& TexturePath, Texture* InTexture);

	private:
		static DXGI_FORMAT FindBestFitFormat(uint32 BitsPerPixel, uint32 RedMask, uint32 GreenMask, uint32 BlueMask, bool HasTransparency);
		/*
		struct GUID_To_DXGI
		{
			GUID WIC;
			DXGI_FORMAT DXGI;
		};

		static const std::vector<GUID_To_DXGI> s_LookupTable;
		*/
	private:
		TextureLoader() = default;
		TextureLoader(const TextureLoader&) = default;
		TextureLoader& operator=(const TextureLoader&) = default;
	};

}

#endif // !TEXTURELOADER_H



