#ifndef GAMEASSET_H
#define GAMEASSET_H

#include "Dwarframe/Editor/EditorExtender.h"

namespace Dwarframe {
	
	class GameAsset : public IResourceInspectorExtender
	{
	IMPLEMENT_SIMPLE_RTTI(GameAsset)

	public:
		GameAsset(std::string AssetPath, std::string AssetName)
			: m_AssetName(AssetName), m_AssetPath(AssetPath), m_Hash(std::hash<std::string>{}(m_AssetPath)), m_AssetSize(0), m_bAssetLoadedCPU(false), m_bAssetLoadedGPU(false) {}

		~GameAsset()
		{
			assert(m_AssetSize == 0 && m_bAssetLoadedCPU == false && m_bAssetLoadedGPU == false);
		}

		virtual bool LoadAsset() = 0;
		virtual bool SaveAsset() = 0;

		bool operator==(const GameAsset& Other) const { return m_Hash == Other.m_Hash; }
		bool operator!=(const GameAsset& Other) const { return m_Hash != Other.m_Hash; }

	public:
		inline std::string GetAssetName() const { return m_AssetName; }
		inline std::string GetAssetPath() const { return m_AssetPath; }

		inline bool IsLoadedCPU() const { return m_bAssetLoadedCPU; }
		inline bool IsLoadedGPU() const { return m_bAssetLoadedGPU; }
		
		inline uint64 GetAssetSize() const { return m_AssetSize; }

	protected:
		std::string m_AssetName;
		std::string m_AssetPath;
		HashValue m_Hash; // What exactly? AssetName? AssetPath? Or maybe both?
		uint64 m_AssetSize;

		// TODO: In the future it should be bitset.
		bool m_bAssetLoadedCPU;
		bool m_bAssetLoadedGPU;
	};

}

#endif // !GAMEASSET_H
