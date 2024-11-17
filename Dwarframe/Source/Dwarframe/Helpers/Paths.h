#ifndef PATHS_H
#define PATHS_H

#include "Dwarframe/Core.h"

namespace Dwarframe {

	class Paths
	{
	public:
		inline static void InitializePaths();
		inline static std::filesystem::path AppendToPath(std::filesystem::path& Path, std::string StringToAppend);
		inline static void SearchPath(std::filesystem::path StartPath, std::filesystem::path& OutPath, std::string SearchedObject, bool SearchOut = false);

		inline static std::filesystem::path GetExeDir() { return ExeDir; }
		inline static std::filesystem::path GetMainDir() { return MainDir; }
		inline static std::filesystem::path GetResourcesDir() { return ResourcesDir; }
		inline static std::filesystem::path GetConfigDir() { return ConfigDir; }
		inline static std::filesystem::path GetTexturesDir() { return TexturesDir; }
		inline static std::filesystem::path GetMeshesDir() { return MeshesDir; }

	private:
		inline static std::filesystem::path ExeDir;
		inline static std::filesystem::path MainDir;
		inline static std::filesystem::path ResourcesDir;
		inline static std::filesystem::path ConfigDir;

		inline static std::filesystem::path TexturesDir;
		inline static std::filesystem::path MeshesDir;
	};

	void Paths::InitializePaths()
	{
        ExeDir = std::filesystem::current_path();

		SearchPath(ExeDir, ConfigDir, "Config", true);
		MainDir = ConfigDir.parent_path();

		SearchPath(ExeDir, ResourcesDir, "Resources", true);
		SearchPath(ResourcesDir, MeshesDir, "Meshes");
		SearchPath(ResourcesDir, TexturesDir, "Textures");
	}

	inline std::filesystem::path Paths::AppendToPath(std::filesystem::path& Path, std::string StringToAppend)
	{
		std::filesystem::path Copy = Path;
		return Copy.append(StringToAppend);
	}

	void Paths::SearchPath(std::filesystem::path StartPath, std::filesystem::path& OutPath, std::string SearchedObject, bool SearchOut)
	{
		std::vector<std::filesystem::path> PathsToSearch;
		PathsToSearch.push_back(StartPath);

		bool bFoundPath = false;
		while (!PathsToSearch.empty())
		{
			StartPath = PathsToSearch.back();
			PathsToSearch.pop_back();

			for (const auto& Entry : std::filesystem::directory_iterator(StartPath))
			{
				if (Entry.path().filename() == SearchedObject)
				{
					OutPath = Entry;
					bFoundPath = true;
					break;
				}
				else if (!SearchOut)
				{
					PathsToSearch.push_back(Entry);
				}
			}

			if (bFoundPath)
			{
				break;
			}

			if (SearchOut)
			{
				PathsToSearch.push_back(StartPath.parent_path());
			}
		}
	}
}

#endif // !PATHS_H