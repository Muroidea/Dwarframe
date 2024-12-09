#ifndef MESHLOADER_H
#define MESHLOADER_H

#include "Dwarframe/Resources/Mesh.h"

namespace Dwarframe {

	class VertexLayout;

	class MeshLoader 
	{
	public:
		static bool Load(std::filesystem::path MeshPath, Mesh* InMesh, bool TryToUseSmallIndices = true);
		//static bool LoadCube(Mesh* InMesh);
		//static void BuildMeshlets(Mesh* InMesh);
		static void BuildMeshletsWithCheck(Mesh* InMesh, bool OnlyLODs = false, bool Scan = false, bool Uniform = false);

		static void AdjustMeshToVertexLayout(Mesh* InMesh, VertexLayout* NewVertexLayout);
		static void GenerateLODs(Mesh* InMesh, std::vector<Mesh::LODSetup>& LODSetups);

	private:
		MeshLoader() = delete;
		MeshLoader(MeshLoader& Other) = delete;
		MeshLoader(MeshLoader&& Other) = delete;
	};

}

#endif // !MESHLOADER_H