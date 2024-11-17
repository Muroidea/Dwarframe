#include "pch.h"
#include "TextureLoader.h"

#include "Dwarframe/Resources/Texture.h"
#include "Dwarframe/Core.h"

#include "FreeImage.h"

namespace Dwarframe {
/*
	const std::vector<TextureLoader::GUID_To_DXGI> TextureLoader::s_LookupTable = {
		{ GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM },
		{ GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM }
	};
	*/
	bool TextureLoader::Load(const std::filesystem::path& TexturePath, Texture* InTexture)
	{
		FREE_IMAGE_FORMAT ImageFormat = FIF_UNKNOWN;
		FIBITMAP* ImageHandle(0);

		ImageFormat = FreeImage_GetFileType(TexturePath.string().c_str(), 0);
		if (ImageFormat == FIF_UNKNOWN)
		{
			ImageFormat = FreeImage_GetFIFFromFilename(TexturePath.string().c_str());
		}

		if (ImageFormat == FIF_UNKNOWN)
		{
			FreeImage_Unload(ImageHandle);
			return false;
		}

		if (FreeImage_FIFSupportsReading(ImageFormat))
		{
			ImageHandle = FreeImage_Load(ImageFormat, TexturePath.string().c_str());
		}
		else
		{
			FreeImage_Unload(ImageHandle);
			return false;
		}

		InTexture->m_BitsPerPixel = FreeImage_GetBPP(ImageHandle);
		InTexture->m_Width = FreeImage_GetWidth(ImageHandle);
		InTexture->m_Height = FreeImage_GetHeight(ImageHandle);

		InTexture->m_Stride = FreeImage_GetPitch(ImageHandle);
		InTexture->m_SizeInBytes = FreeImage_GetDIBSize(ImageHandle);

		FREE_IMAGE_TYPE ImageType = FreeImage_GetImageType(ImageHandle);
		//uint32 UsedColors = FreeImage_GetColorsUsed(ImageHandle);
		uint32 RedChannelMask = FreeImage_GetRedMask(ImageHandle);
		uint32 GreenChannelMask = FreeImage_GetGreenMask(ImageHandle);
		uint32 BlueChannelMask = FreeImage_GetBlueMask(ImageHandle);
		bool Trasparency = FreeImage_IsTransparent(ImageHandle);
		InTexture->m_PixelFormat = FindBestFitFormat(InTexture->m_BitsPerPixel, RedChannelMask, GreenChannelMask, BlueChannelMask, Trasparency);
		
		InTexture->m_Data = FreeImage_GetBits(ImageHandle);

		//Free FreeImage's copy of the data
		FreeImage_Unload(ImageHandle);

		/*
		ComPtr<IWICImagingFactory> WICFactory; 
		ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&WICFactory)), "Creation of WIC factory failed.");

		ComPtr<IWICStream> WICStream;
		ThrowIfFailed(WICFactory->CreateStream(&WICStream), "Creation of WIC stream failed.");
		ThrowIfFailed(WICStream->InitializeFromFilename(ImagePath.wstring().c_str(), GENERIC_READ), "Cannot open file.");

		ComPtr<IWICBitmapDecoder> WICBitmapDecoder;
		ThrowIfFailed(WICFactory->CreateDecoderFromStream(WICStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &WICBitmapDecoder), "Creation of decode failed.");

		ComPtr<IWICBitmapFrameDecode> WICBitmapFrameDecode;
		WICBitmapDecoder->GetFrame(0, &WICBitmapFrameDecode);
		

		WICBitmapFrameDecode->GetSize(&Data.Width, &Data.Height);
		WICBitmapFrameDecode->GetPixelFormat(&Data.WICPixelFormat);
		
		ComPtr<IWICComponentInfo> WICComponentInfo;
		WICFactory->CreateComponentInfo(Data.WICPixelFormat, &WICComponentInfo);

		ComPtr<IWICPixelFormatInfo> WICPixelFormatInfo;
		WICComponentInfo->QueryInterface(IID_PPV_ARGS(&WICPixelFormatInfo));

		WICPixelFormatInfo->GetBitsPerPixel(&Data.BitsPerPixel);
		WICPixelFormatInfo->GetChannelCount(&Data.ChannelCount);

		Data.Stride = Data.Width * ((Data.BitsPerPixel + 7) / 8);
		Data.SizeInBytes = Data.Height * Data.Stride;
		Data.Data = malloc(Data.SizeInBytes);

		WICRect Rect;
		Rect.Width = Data.Width;
		Rect.Height = Data.Height;
		Rect.X = Rect.Y = 0;

		WICBitmapFrameDecode->CopyPixels(&Rect, Data.Stride, Data.SizeInBytes, (unsigned char*)Data.Data);
		
		auto FindIt = std::find_if(s_LookupTable.begin(), s_LookupTable.end(), [&](const GUID_To_DXGI& Entry) {
			return memcmp(&Entry.WIC, &Data.WICPixelFormat, sizeof(GUID)) == 0;
		});
		
		if (FindIt == s_LookupTable.end())
		{
			return false;
		}
		Data.PixelFormat = FindIt->DXGI;

		WICPixelFormatInfo.Reset();
		WICComponentInfo.Reset();
		WICBitmapFrameDecode.Reset();
		WICBitmapDecoder.Reset();
		WICStream.Reset();
		WICFactory.Reset();
		*/
		return true;
	}

	DXGI_FORMAT TextureLoader::FindBestFitFormat(uint32 BitsPerPixel, uint32 RedMask, uint32 GreenMask, uint32 BlueMask, bool HasTransparency)
	{
		uint32 Masks[3] = { RedMask, GreenMask, BlueMask };
		uint32 Positions[3] = { 0, 0, 0 };
		uint32 NumOfBits[3] = { 0, 0, 0 };
		uint32 NumOfChannels = 0;
		
		for (uint32 Mask = 0; Mask < 3; Mask++)
		{
			if (Masks[Mask] > 0)
			{
				NumOfChannels++;
			}
		}

		if (HasTransparency)
		{
			NumOfChannels++;
		}

		for (uint32 Mask = 0; Mask < 3; Mask++)
		{
			for (uint32 Bit = 0; Bit < 32; Bit++)
			{
				if (Masks[Mask] & (1 << Bit))
				{
					NumOfBits[Mask]++;
				}
			}
		}
		
		if ((Masks[0] < Masks[2] && Masks[2] > 0) || (Masks[0] > Masks[1] && Masks[1] > 0))
		{
			Positions[0] = 0;
			Positions[1] = 1;
			Positions[2] = 2;
		}
		else
		{
			Positions[0] = 2;
			Positions[1] = 1;
			Positions[2] = 0;
		}
		
		if (BitsPerPixel == 32 && (NumOfChannels == 3 || NumOfChannels == 4) && Positions[0] == 0)
		{
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		else if (BitsPerPixel == 32 && (NumOfChannels == 3 || NumOfChannels == 4) && Positions[0] == 2)
		{
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		}
		else
		{
			return DXGI_FORMAT_UNKNOWN;
		}
	}
}
