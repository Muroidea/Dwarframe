#include "pch.h"
#include "Texture.h"

#include "ImGUI/imgui.h"

namespace Dwarframe {

	Texture::Texture(std::string AssetPath, std::string AssetName)
		: GameAsset(AssetPath, AssetName)
	{
		
	}

	bool Texture::LoadAsset()
	{
		return false;
	}

	bool Texture::SaveAsset()
	{
		return false;
	}

#if WITH_EDITOR
	void Texture::RenderProperties()
	{
		ImGui::Text("Texture info:");
	}
#endif
}
