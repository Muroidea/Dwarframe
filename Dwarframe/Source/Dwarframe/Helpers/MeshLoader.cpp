#include "pch.h"
#include "MeshLoader.h"

#include "Dwarframe/Resources/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "meshoptimizer/src/meshoptimizer.h"

namespace Dwarframe {

	bool MeshLoader::Load(std::filesystem::path MeshPath, Mesh* InMesh, bool TryToUseSmallIndices)
	{
		Assimp::Importer AssimpImporter;
		const aiScene* AssimpScene;
		
		uint32 ReadFlags = 
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ValidateDataStructure |
			aiProcess_ImproveCacheLocality |
			aiProcess_FixInfacingNormals |
			aiProcess_FindInvalidData;

		AssimpScene = AssimpImporter.ReadFile(MeshPath.string(), ReadFlags);
		if (AssimpScene == nullptr)
		{
			// TODO: Better logging!
			printf("\nMeshLoader: Error during importing mesh:\n %s:\n %s", *MeshPath.c_str(), AssimpImporter.GetErrorString());
			return false;
		}
		
		InMesh->m_SubmeshList.clear();

		uint32 NumVertices = 0;
		uint32 NumIndices = 0;

		uint32 NumVerticesAll = 0;
		uint32 NumIndicesAll = 0;

		for (uint32 SubmeshID = 0; SubmeshID < AssimpScene->mNumMeshes; SubmeshID++)
		{
			// Setup general info about submesh
			InMesh->m_SubmeshList.emplace_back();
			Submesh& CurrentSubmesh = InMesh->m_SubmeshList[SubmeshID];

			NumVertices = AssimpScene->mMeshes[SubmeshID]->mNumVertices;
			NumIndices = AssimpScene->mMeshes[SubmeshID]->mNumFaces * 3;

			// Copy info about vertices attributes
			const aiMesh* Mesh = AssimpScene->mMeshes[SubmeshID];

			struct AttributeCopyInfo
			{
				float32* Src = nullptr;
				uint32 Size = 0;
			} CopyInfo;

			std::map<EAttribute, AttributeCopyInfo> CopyInfoMap;
			uint32 VertexStride = 0;

			// Check what attributes do mesh have to calculate required memory
			
			// Position
			CopyInfo = { reinterpret_cast<float32*>(Mesh->mVertices), GetAttributeSizeInBytes(EAttribute::Position) };
			CopyInfoMap.emplace(EAttribute::Position, CopyInfo);
			VertexStride += GetAttributeNumOfFloats(EAttribute::Position);
			
			// Normals
			if (Mesh->mNormals)
			{
				CopyInfo = { reinterpret_cast<float32*>(Mesh->mNormals), GetAttributeSizeInBytes(EAttribute::Normal) };
				CopyInfoMap.emplace(EAttribute::Normal, CopyInfo);

				VertexStride += GetAttributeNumOfFloats(EAttribute::Normal);
				CurrentSubmesh.Attributes.AddAttribute(EAttribute::Normal);
			}

			/*
			// Tangents
			if (Mesh->mTangents)
			{
				CopyInfo = { reinterpret_cast<float32*>(Mesh->mTangents), GetAttributeSizeInBytes(EAttribute::Tangent) };
				CopyInfoMap.emplace(EAttribute::Tangent, CopyInfo);

				VertexStride += GetAttributeNumOfFloats(EAttribute::Tangent);
				CurrentSubmesh.Attributes.AddAttribute(EAttribute::Tangent);
			}

			// Bitangents
			if (Mesh->mBitangents)
			{
				CopyInfo = { reinterpret_cast<float32*>(Mesh->mBitangents), GetAttributeSizeInBytes(EAttribute::Bitangent) };
				CopyInfoMap.emplace(EAttribute::Bitangent, CopyInfo);

				VertexStride += GetAttributeNumOfFloats(EAttribute::Bitangent);
				CurrentSubmesh.Attributes.AddAttribute(EAttribute::Bitangent);
			}

			// TexCoords
			for (uint32 TexCoordID = 0; TexCoordID < MaxNumberOfTextureCoords; TexCoordID++)
			{
				if (Mesh->mTextureCoords[TexCoordID])
				{
					CopyInfo = { reinterpret_cast<float32*>(Mesh->mTextureCoords[TexCoordID]), GetAttributeSizeInBytes(EAttribute::TexCoord_0) };
					CopyInfoMap.emplace(static_cast<EAttribute>(EAttribute::TexCoord_0 + TexCoordID), CopyInfo);

					VertexStride += GetAttributeNumOfFloats(EAttribute::TexCoord_0);
					CurrentSubmesh.Attributes.AddAttribute(static_cast<EAttribute>(EAttribute::TexCoord_0 + TexCoordID));
				}
			}
			*/

			// Allocate required memory
			//CurrentSubmesh.MeshVertices = reinterpret_cast<float32*>(malloc(NumVertices * VertexStride * sizeof(float32)));
			CurrentSubmesh.MeshVertices.resize(NumVertices * VertexStride);

			// Copy attributes data
			for (uint32 VertexID = 0; VertexID < NumVertices; VertexID++)
			{
				uint32 VertexFloat = 0;
				for (uint32 AttrID = 0; AttrID < EAttribute::Count; AttrID++)
				{
					auto Value = CopyInfoMap.find(static_cast<EAttribute>(AttrID));

					if (Value != CopyInfoMap.end())
					{
						uint32 NumOfFloats = GetAttributeNumOfFloats(Value->first);
						memcpy(&CurrentSubmesh.MeshVertices[VertexID * VertexStride + VertexFloat], &Value->second.Src[VertexID * NumOfFloats], Value->second.Size);
						VertexFloat += NumOfFloats;
					}
				}
			}

			// Copy indices
			if (CurrentSubmesh.bUseSmallIndices)
			{
				/*
				CurrentSubmesh.SmallIndicesData = reinterpret_cast<uint16*>(malloc(NumIndices * sizeof(uint16)));
				for (uint32 FaceID = 0; FaceID < Mesh->mNumFaces; FaceID++)
				{
					const aiFace& Face = Mesh->mFaces[FaceID];
					CurrentSubmesh.SmallIndicesData[FaceID * 3 + 0] = Face.mIndices[0];
					CurrentSubmesh.SmallIndicesData[FaceID * 3 + 1] = Face.mIndices[1];
					CurrentSubmesh.SmallIndicesData[FaceID * 3 + 2] = Face.mIndices[2];
				}
				*/
			}
			else
			{
				//CurrentSubmesh.BigIndicesData = reinterpret_cast<uint32*>(malloc(NumIndices * sizeof(uint32)));
				CurrentSubmesh.BigIndicesData.resize(NumIndices);
				for (uint32 FaceID = 0; FaceID < Mesh->mNumFaces; FaceID++)
				{
					const aiFace& Face = Mesh->mFaces[FaceID];
					assert(Face.mNumIndices == 3);
					memcpy(&CurrentSubmesh.BigIndicesData[FaceID * 3], Face.mIndices, 3 * sizeof(uint32));
				}
			}

			
			std::vector<uint32> Remap;
			Remap.resize(NumIndices);

			//uint64 TotalVertices = meshopt_generateVertexRemap(Remap.data(), CurrentSubmesh.BigIndicesData, NumIndices, CurrentSubmesh.MeshVertices, NumVertices, CurrentSubmesh.Attributes.GetVertexStrideInBytes());
			uint64 TotalVertices = meshopt_generateVertexRemap(Remap.data(), 
				CurrentSubmesh.BigIndicesData.data(), NumIndices, 
				CurrentSubmesh.MeshVertices.data(), NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes());
			
			//uint32* ResultIndices = reinterpret_cast<uint32*>(malloc(NumIndices * sizeof(uint32)));
			std::vector<uint32> ResultIndices;
			ResultIndices.resize(NumIndices);
			meshopt_remapIndexBuffer(ResultIndices.data(), 
				CurrentSubmesh.BigIndicesData.data(), NumIndices, 
				Remap.data());

			//float32* ResultVertices = reinterpret_cast<float*>(malloc(TotalVertices * VertexStride * sizeof(float32)));
			std::vector<float32> ResultVertices;
			ResultVertices.resize(TotalVertices * VertexStride);
			meshopt_remapVertexBuffer(ResultVertices.data(), 
				CurrentSubmesh.MeshVertices.data(), NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes(), Remap.data());
		
			//delete[] CurrentSubmesh.BigIndicesData;
			//delete[] CurrentSubmesh.MeshVertices;

			CurrentSubmesh.BigIndicesData = std::move(ResultIndices);
			CurrentSubmesh.MeshVertices = std::move(ResultVertices);
			

			InMesh->m_AssetSize = TotalVertices * VertexStride * sizeof(float32);
			//InMesh->m_AssetSize = VertexStride * NumVertices * sizeof(float32);
			CurrentSubmesh.NumOfVertices = TotalVertices;
			//CurrentSubmesh.NumOfVertices = NumVertices;
			CurrentSubmesh.NumOfIndices = NumIndices;

			NumVerticesAll += CurrentSubmesh.NumOfVertices;
			NumIndicesAll += CurrentSubmesh.NumOfIndices;
		}

		InMesh->m_bAssetLoadedCPU = true;
		AssimpImporter.FreeScene();

		return true;
	}

	bool MeshLoader::LoadCube(Mesh* InMesh)
	{
		InMesh->m_SubmeshList.clear();

		uint32 NumVertices = 0;
		uint32 NumIndices = 0;

		uint32 NumVerticesAll = 0;
		uint32 NumIndicesAll = 0;

		for (uint32 SubmeshID = 0; SubmeshID < 1; SubmeshID++)
		{
			// Setup general info about submesh
			InMesh->m_SubmeshList.emplace_back();
			Submesh& CurrentSubmesh = InMesh->m_SubmeshList[SubmeshID];

			NumVertices = 8;
			NumIndices = 12 * 3;

			uint32 VertexStride = 0;

			// Position
			VertexStride += GetAttributeNumOfFloats(EAttribute::Position);

			CurrentSubmesh.MeshVertices = {
				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};


			CurrentSubmesh.BigIndicesData = {
				0, 1, 2,    // side 1
				2, 1, 3,
				4, 0, 6,    // side 2
				6, 0, 2,
				7, 5, 6,    // side 3
				6, 5, 4,
				3, 1, 7,    // side 4
				7, 1, 5,
				4, 5, 0,    // side 5
				0, 5, 1,
				3, 7, 2,    // side 6
				2, 7, 6
			};

			std::vector<uint32> Remap;
			Remap.resize(NumIndices);

			//uint64 TotalVertices = meshopt_generateVertexRemap(Remap.data(), CurrentSubmesh.BigIndicesData, NumIndices, CurrentSubmesh.MeshVertices, NumVertices, CurrentSubmesh.Attributes.GetVertexStrideInBytes());
			uint64 TotalVertices = meshopt_generateVertexRemap(Remap.data(), 
				CurrentSubmesh.BigIndicesData.data(), NumIndices, 
				CurrentSubmesh.MeshVertices.data(), NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes());
			
			//uint32* ResultIndices = reinterpret_cast<uint32*>(malloc(NumIndices * sizeof(uint32)));
			std::vector<uint32> ResultIndices;
			ResultIndices.resize(NumIndices);
			meshopt_remapIndexBuffer(ResultIndices.data(), 
				CurrentSubmesh.BigIndicesData.data(), NumIndices, 
				Remap.data());

			//float32* ResultVertices = reinterpret_cast<float*>(malloc(VertexStride * TotalVertices * sizeof(float32)));
			std::vector<float32> ResultVertices;
			ResultVertices.resize(TotalVertices * 3);
			meshopt_remapVertexBuffer(ResultVertices.data(), 
				CurrentSubmesh.MeshVertices.data(), NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes(), Remap.data());
		
			//delete[] CurrentSubmesh.BigIndicesData;
			//delete[] CurrentSubmesh.MeshVertices;

			CurrentSubmesh.BigIndicesData = std::move(ResultIndices);
			CurrentSubmesh.MeshVertices = std::move(ResultVertices);
			

			InMesh->m_AssetSize = VertexStride * TotalVertices * sizeof(float32);
			//InMesh->m_AssetSize = VertexStride * NumVertices * sizeof(float32);
			CurrentSubmesh.NumOfVertices = TotalVertices;
			//CurrentSubmesh.NumOfVertices = NumVertices;
			CurrentSubmesh.NumOfIndices = NumIndices;

			NumVerticesAll += CurrentSubmesh.NumOfVertices;
			NumIndicesAll += CurrentSubmesh.NumOfIndices;
		}

		InMesh->m_bAssetLoadedCPU = true;

		return true;
	}

	void MeshLoader::BuildMeshlets(Mesh* InMesh)
	{
		const uint64 MaxVertices = 64;
		const uint64 MaxTriangles = 124;
		const float ConeWeight = 0.0f;

		for (uint32 SubmeshID = 0; SubmeshID < InMesh->m_SubmeshList.size(); SubmeshID++)
		{
			Submesh& CurrentSubmesh = InMesh->m_SubmeshList[SubmeshID];

			meshopt_optimizeVertexCache(CurrentSubmesh.BigIndicesData.data(), 
				CurrentSubmesh.BigIndicesData.data(), 
				CurrentSubmesh.BigIndicesData.size(), 
				CurrentSubmesh.MeshVertices.size());

			meshopt_optimizeVertexFetch(CurrentSubmesh.MeshVertices.data(), 
				CurrentSubmesh.BigIndicesData.data(), 
				CurrentSubmesh.BigIndicesData.size(), 
				CurrentSubmesh.MeshVertices.data(), 
				CurrentSubmesh.MeshVertices.size(), 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes());

			uint64 MaxMeshlets = meshopt_buildMeshletsBound(CurrentSubmesh.NumOfIndices, MaxVertices, MaxTriangles);

			std::vector<meshopt_Meshlet> Meshlets(MaxMeshlets);
			CurrentSubmesh.MeshletVertexIndices.resize(MaxMeshlets * MaxVertices);
			std::vector<uint8> MeshletTriangles(MaxMeshlets * MaxTriangles * 3);

			uint64 MeshletCount;
			if (CurrentSubmesh.bUseSmallIndices)
			{

			}
			else
			{
				MeshletCount = meshopt_buildMeshlets(Meshlets.data(), CurrentSubmesh.MeshletVertexIndices.data(), MeshletTriangles.data(), 
					CurrentSubmesh.BigIndicesData.data(),
					CurrentSubmesh.NumOfIndices, 
					CurrentSubmesh.MeshVertices.data(), 
					CurrentSubmesh.NumOfVertices, 
					CurrentSubmesh.Attributes.GetVertexStrideInBytes(),
					MaxVertices, MaxTriangles, ConeWeight);
			}

			// Trim arrays
			const meshopt_Meshlet& LastMeshlet = Meshlets[MeshletCount - 1];

			CurrentSubmesh.MeshletVertexIndices.resize(LastMeshlet.vertex_offset + LastMeshlet.vertex_count);
			MeshletTriangles.resize(LastMeshlet.triangle_offset + ((LastMeshlet.triangle_count * 3 + 3) & ~3));
			//MeshletTriangles.resize(LastMeshlet.triangle_offset + LastMeshlet.triangle_count * 3);
			Meshlets.resize(MeshletCount);

			// Functionality added in newer library version
			// Further opimization per meshlet
			/*
			for (uint32 MeshletID = 0; MeshletID < MeshletCount; MeshletID++)
			{
				const meshopt_Meshlet& Meshlet = Meshlets[MeshletID]; 
				meshopt_optimizeMeshlet(&MeshletVertices[Meshlet.vertex_offset], &MeshletTriangles[Meshlet.triangle_offset], Meshlet.triangle_count, Meshlet.vertex_count);
			}
			*/

			// Fit meshopt meshlets into internal formats
			CurrentSubmesh.MeshletTriangles.resize(MeshletTriangles.size() / 3);
			CurrentSubmesh.Meshlets.resize(Meshlets.size());
			for (uint32 MeshletID = 0; MeshletID < Meshlets.size(); MeshletID++)
			{
				const uint32 SpanEnd = Meshlets[MeshletID].triangle_offset + Meshlets[MeshletID].triangle_count * 3;
				for (uint32 TriangleID = Meshlets[MeshletID].triangle_offset; TriangleID < SpanEnd; TriangleID += 3)
				{
					Submesh::MeshletTriangle NewTriangle;
					NewTriangle.Index0 = MeshletTriangles[TriangleID + 0];
					NewTriangle.Index1 = MeshletTriangles[TriangleID + 1];
					NewTriangle.Index2 = MeshletTriangles[TriangleID + 2];

					CurrentSubmesh.MeshletTriangles[TriangleID / 3] = NewTriangle;
				}

				CurrentSubmesh.Meshlets[MeshletID].TriangleOffset = Meshlets[MeshletID].triangle_offset / 3;
				CurrentSubmesh.Meshlets[MeshletID].VertexOffset = Meshlets[MeshletID].vertex_offset;

				CurrentSubmesh.Meshlets[MeshletID].TriangleCount = Meshlets[MeshletID].triangle_count;
				CurrentSubmesh.Meshlets[MeshletID].VertexCount = Meshlets[MeshletID].vertex_count;
			}
		}
	}

	void MeshLoader::BuildMeshletsWithCheck(Mesh* InMesh, bool Scan, bool Uniform)
	{
		// NVidia-recommends 64/126; we round 126 down to a multiple of 4
		// alternatively we also test uniform configuration with 64/64 which is better for AMD
		const uint64 MaxVertices = 64;
		const uint64 MaxTriangles = Uniform ? 64 : 124;

		// note: should be set to 0 unless cone culling is used at runtime!
		const float ConeWeight = 0.0f;

		for (uint32 SubmeshID = 0; SubmeshID < InMesh->m_SubmeshList.size(); SubmeshID++)
		{
			Submesh& CurrentSubmesh = InMesh->m_SubmeshList[SubmeshID];

			uint64 MaxMeshlets = meshopt_buildMeshletsBound(CurrentSubmesh.NumOfIndices, MaxVertices, MaxTriangles);

			std::vector<meshopt_Meshlet> Meshlets(MaxMeshlets);
			CurrentSubmesh.MeshletVertexIndices.resize(MaxMeshlets * MaxVertices);
			std::vector<uint8> MeshletTriangles(MaxMeshlets * MaxTriangles * 3);

			uint64 MeshletCount;
			if (Scan)
			{
				if (CurrentSubmesh.bUseSmallIndices)
				{

				}
				else
				{
					MeshletCount = meshopt_buildMeshletsScan(Meshlets.data(), CurrentSubmesh.MeshletVertexIndices.data(), MeshletTriangles.data(),
						CurrentSubmesh.BigIndicesData.data(),
						CurrentSubmesh.NumOfIndices,
						CurrentSubmesh.NumOfVertices,
						MaxVertices, MaxTriangles);
				}
			}
			else
			{
				if (CurrentSubmesh.bUseSmallIndices)
				{

				}
				else
				{
					MeshletCount = meshopt_buildMeshlets(Meshlets.data(), CurrentSubmesh.MeshletVertexIndices.data(), MeshletTriangles.data(),
						CurrentSubmesh.BigIndicesData.data(),
						CurrentSubmesh.NumOfIndices,
						CurrentSubmesh.MeshVertices.data(),
						CurrentSubmesh.NumOfVertices,
						CurrentSubmesh.Attributes.GetVertexStrideInBytes(),
						MaxVertices, MaxTriangles, ConeWeight);
				}
			}
			Meshlets.resize(MeshletCount);

			for (uint64 i = 0; i < Meshlets.size(); i++)
			{
				meshopt_optimizeMeshlet(
					&CurrentSubmesh.MeshletVertexIndices[Meshlets[i].vertex_offset], 
					&MeshletTriangles[Meshlets[i].triangle_offset], 
					Meshlets[i].triangle_count, Meshlets[i].vertex_count);
			}

			if (Meshlets.size())
			{
				const meshopt_Meshlet& Last = Meshlets.back();

				// this is an example of how to trim the vertex/triangle arrays when copying data out to GPU storage
				CurrentSubmesh.MeshletVertexIndices.resize(Last.vertex_offset + Last.vertex_count);
				MeshletTriangles.resize(Last.triangle_offset + ((Last.triangle_count * 3 + 3) & ~3));
				//MeshletTriangles.resize(Last.triangle_offset + Last.triangle_count * 3);
			}

			// Fit meshopt meshlets into internal formats
			CurrentSubmesh.MeshletTriangles.resize(MeshletTriangles.size() / 3);
			CurrentSubmesh.Meshlets.resize(Meshlets.size());
			for (uint32 MeshletID = 0; MeshletID < Meshlets.size(); MeshletID++)
			{
				const uint32 SpanEnd = Meshlets[MeshletID].triangle_offset + Meshlets[MeshletID].triangle_count * 3;
				for (uint32 TriangleID = Meshlets[MeshletID].triangle_offset; TriangleID < SpanEnd; TriangleID += 3)
				{
					Submesh::MeshletTriangle NewTriangle;
					NewTriangle.Index0 = MeshletTriangles[TriangleID + 0];
					NewTriangle.Index1 = MeshletTriangles[TriangleID + 1];
					NewTriangle.Index2 = MeshletTriangles[TriangleID + 2];

					CurrentSubmesh.MeshletTriangles[TriangleID / 3] = NewTriangle;
				}

				CurrentSubmesh.Meshlets[MeshletID].VertexCount = Meshlets[MeshletID].vertex_count;
				CurrentSubmesh.Meshlets[MeshletID].VertexOffset = Meshlets[MeshletID].vertex_offset;

				CurrentSubmesh.Meshlets[MeshletID].TriangleCount = Meshlets[MeshletID].triangle_count;
				CurrentSubmesh.Meshlets[MeshletID].TriangleOffset = Meshlets[MeshletID].triangle_offset / 3;

			}
		}
		/*
		// note: input mesh is assumed to be optimized for vertex cache and vertex fetch
		size_t max_meshlets = meshopt_buildMeshletsBound(InMesh.indices.size(), max_vertices, max_triangles);
		std::vector<meshopt_Meshlet> meshlets(max_meshlets);
		std::vector<unsigned int> meshlet_vertices(max_meshlets * max_vertices);
		std::vector<unsigned char> meshlet_triangles(max_meshlets * max_triangles * 3);

		if (scan)
			meshlets.resize(meshopt_buildMeshletsScan(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size(), max_vertices, max_triangles));
		else
			meshlets.resize(meshopt_buildMeshlets(&meshlets[0], &meshlet_vertices[0], &meshlet_triangles[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), max_vertices, max_triangles, cone_weight));

		for (size_t i = 0; i < meshlets.size(); ++i)
			meshopt_optimizeMeshlet(&meshlet_vertices[meshlets[i].vertex_offset], &meshlet_triangles[meshlets[i].triangle_offset], meshlets[i].triangle_count, meshlets[i].vertex_count);

		if (meshlets.size())
		{
			const meshopt_Meshlet& last = meshlets.back();

			// this is an example of how to trim the vertex/triangle arrays when copying data out to GPU storage
			meshlet_vertices.resize(last.vertex_offset + last.vertex_count);
			meshlet_triangles.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));
		}

		double end = timestamp();

		double avg_vertices = 0;
		double avg_triangles = 0;
		double avg_boundary = 0;
		size_t not_full = 0;
		size_t not_connected = 0;

		std::vector<int> boundary(mesh.vertices.size());

		for (size_t i = 0; i < meshlets.size(); ++i)
		{
			const meshopt_Meshlet& m = meshlets[i];

			for (unsigned int j = 0; j < m.vertex_count; ++j)
				boundary[meshlet_vertices[m.vertex_offset + j]]++;
		}

		for (size_t i = 0; i < meshlets.size(); ++i)
		{
			const meshopt_Meshlet& m = meshlets[i];

			avg_vertices += m.vertex_count;
			avg_triangles += m.triangle_count;
			not_full += uniform ? m.triangle_count < max_triangles : m.vertex_count < max_vertices;

			for (unsigned int j = 0; j < m.vertex_count; ++j)
				if (boundary[meshlet_vertices[m.vertex_offset + j]] > 1)
					avg_boundary += 1;

			// union-find vertices to check if the meshlet is connected
			int parents[max_vertices];
			for (unsigned int j = 0; j < m.vertex_count; ++j)
				parents[j] = int(j);

			for (unsigned int j = 0; j < m.triangle_count * 3; ++j)
			{
				int v0 = meshlet_triangles[m.triangle_offset + j];
				int v1 = meshlet_triangles[m.triangle_offset + j + (j % 3 == 2 ? -2 : 1)];

				v0 = follow(parents, v0);
				v1 = follow(parents, v1);

				parents[v0] = v1;
			}

			int roots = 0;
			for (unsigned int j = 0; j < m.vertex_count; ++j)
				roots += follow(parents, j) == int(j);

			assert(roots != 0);
			not_connected += roots > 1;
		}

		avg_vertices /= double(meshlets.size());
		avg_triangles /= double(meshlets.size());
		avg_boundary /= double(meshlets.size());

		printf("Meshlets%c: %d meshlets (avg vertices %.1f, avg triangles %.1f, avg boundary %.1f, not full %d, not connected %d) in %.2f msec\n",
			scan ? 'S' : (uniform ? 'U' : ' '),
			int(meshlets.size()), avg_vertices, avg_triangles, avg_boundary, int(not_full), int(not_connected), (end - start) * 1000);

		float camera[3] = { 100, 100, 100 };

		size_t rejected = 0;
		size_t accepted = 0;
		double radius_mean = 0;
		double cone_mean = 0;

		std::vector<float> radii(meshlets.size());
		std::vector<float> cones(meshlets.size());

		double startc = timestamp();
		for (size_t i = 0; i < meshlets.size(); ++i)
		{
			const meshopt_Meshlet& m = meshlets[i];

			meshopt_Bounds bounds = meshopt_computeMeshletBounds(&meshlet_vertices[m.vertex_offset], &meshlet_triangles[m.triangle_offset], m.triangle_count, &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex));

			radii[i] = bounds.radius;
			cones[i] = 90.f - acosf(bounds.cone_cutoff) * (180.f / 3.1415926f);

			radius_mean += radii[i];
			cone_mean += cones[i];

			// trivial accept: we can't ever backface cull this meshlet
			accepted += (bounds.cone_cutoff >= 1);

			// perspective projection: dot(normalize(cone_apex - camera_position), cone_axis) > cone_cutoff
			// alternative formulation for perspective projection that doesn't use apex (and uses cluster bounding sphere instead):
			// dot(normalize(center - camera_position), cone_axis) > cone_cutoff + radius / length(center - camera_position)
			float cview[3] = { bounds.center[0] - camera[0], bounds.center[1] - camera[1], bounds.center[2] - camera[2] };
			float cviewlength = sqrtf(cview[0] * cview[0] + cview[1] * cview[1] + cview[2] * cview[2]);

			rejected += cview[0] * bounds.cone_axis[0] + cview[1] * bounds.cone_axis[1] + cview[2] * bounds.cone_axis[2] >= bounds.cone_cutoff * cviewlength + bounds.radius;
		}
		double endc = timestamp();

		radius_mean /= double(meshlets.size());
		cone_mean /= double(meshlets.size());

		double radius_variance = 0;

		for (size_t i = 0; i < meshlets.size(); ++i)
			radius_variance += (radii[i] - radius_mean) * (radii[i] - radius_mean);

		radius_variance /= double(meshlets.size() - 1);

		double radius_stddev = sqrt(radius_variance);

		size_t meshlets_std = 0;

		for (size_t i = 0; i < meshlets.size(); ++i)
			meshlets_std += radii[i] < radius_mean + radius_stddev;

		printf("Bounds   : radius mean %f stddev %f; %.1f%% meshlets under 1; cone angle %.1f; cone reject %.1f%% trivial accept %.1f%% in %.2f msec\n",
			radius_mean, radius_stddev,
			double(meshlets_std) / double(meshlets.size()) * 100,
			cone_mean, double(rejected) / double(meshlets.size()) * 100, double(accepted) / double(meshlets.size()) * 100,
			(endc - startc) * 1000);
		*/
	}

	void MeshLoader::AdjustMeshToVertexLayout(Mesh* InMesh, VertexLayout* NewVertexLayout)
	{
		
	}
}