#include "pch.h"
#include "MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <meshoptimizer/src/meshoptimizer.h>

namespace Dwarframe {

	bool MeshLoader::Load(std::filesystem::path MeshPath, Mesh* InMesh, bool TryToUseSmallIndices)
	{
		Assimp::Importer AssimpImporter;
		const aiScene* AssimpScene;
		
		uint32 ReadFlags = 
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_GenUVCoords |
			aiProcess_ValidateDataStructure |
			aiProcess_ImproveCacheLocality |
			aiProcess_FixInfacingNormals |
			aiProcess_FindInvalidData;

		AssimpScene = AssimpImporter.ReadFile(MeshPath.string(), ReadFlags);
		if (AssimpScene == nullptr)
		{
			std::string Name = MeshPath.string();
			// TODO: Better logging!
			printf("\nMeshLoader: Error during importing mesh:\n %s:\n %s", *Name.c_str(), AssimpImporter.GetErrorString());
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
			Submesh& CurrentSubmesh = InMesh->m_SubmeshList.back();
			CurrentSubmesh.LODInfos.emplace_back();

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
			*/
			// Allocate required memory
			CurrentSubmesh.MeshVertices = reinterpret_cast<float32*>(malloc(NumVertices * VertexStride * sizeof(float32)));
			CurrentSubmesh.MeshVerticesV.resize(NumVertices * VertexStride);

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
						memcpy(&CurrentSubmesh.MeshVerticesV[VertexID * VertexStride + VertexFloat], &Value->second.Src[VertexID * NumOfFloats], Value->second.Size);
						VertexFloat += NumOfFloats;
					}
				}
			}

			// Copy indices
			if (CurrentSubmesh.bUseSmallIndices)
			{
				CurrentSubmesh.SmallIndices = reinterpret_cast<uint16*>(malloc(NumIndices * sizeof(uint16)));
				for (uint32 FaceID = 0; FaceID < Mesh->mNumFaces; FaceID++)
				{
					const aiFace& Face = Mesh->mFaces[FaceID];
					CurrentSubmesh.SmallIndices[FaceID * 3 + 0] = Face.mIndices[0];
					CurrentSubmesh.SmallIndices[FaceID * 3 + 1] = Face.mIndices[1];
					CurrentSubmesh.SmallIndices[FaceID * 3 + 2] = Face.mIndices[2];
				}
			}
			else
			{
				CurrentSubmesh.BigIndices = reinterpret_cast<uint32*>(malloc(NumIndices * sizeof(uint32)));
				CurrentSubmesh.BigIndicesV.resize(NumIndices);
				for (uint32 FaceID = 0; FaceID < Mesh->mNumFaces; FaceID++)
				{
					const aiFace& Face = Mesh->mFaces[FaceID];
					assert(Face.mNumIndices == 3);
					memcpy(&CurrentSubmesh.BigIndices[FaceID * 3], Face.mIndices, 3 * sizeof(uint32));
					memcpy(&CurrentSubmesh.BigIndicesV[FaceID * 3], Face.mIndices, 3 * sizeof(uint32));
				}
			}
			/*
			uint32* Remap = reinterpret_cast<uint32*>(malloc(NumIndices * sizeof(uint32)));

			uint64 TotalVertices = meshopt_generateVertexRemap(Remap, 
				CurrentSubmesh.BigIndices, NumIndices, 
				CurrentSubmesh.MeshVertices, NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes());
			
			uint32* ResultIndices = reinterpret_cast<uint32*>(malloc(NumIndices * sizeof(uint32)));
			meshopt_remapIndexBuffer(ResultIndices, 
				CurrentSubmesh.BigIndices, NumIndices, 
				Remap);
			
			float32* ResultVertices = reinterpret_cast<float*>(malloc(TotalVertices * VertexStride * sizeof(float32)));
			meshopt_remapVertexBuffer(ResultVertices, 
				CurrentSubmesh.MeshVertices, NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes(), Remap);

			free(CurrentSubmesh.MeshVertices);
			CurrentSubmesh.MeshVertices = ResultVertices;

			free(CurrentSubmesh.BigIndices);
			CurrentSubmesh.BigIndices = ResultIndices;
			*/
			
			std::vector<uint32> Remap(NumIndices);

			uint64 TotalVertices = meshopt_generateVertexRemap(Remap.data(),
				CurrentSubmesh.BigIndicesV.data(), NumIndices,
				CurrentSubmesh.MeshVerticesV.data(), NumVertices,
				CurrentSubmesh.Attributes.GetVertexStrideInBytes());
			
			std::vector<uint32> ResultIndices(NumIndices);
			meshopt_remapIndexBuffer(ResultIndices.data(), 
				CurrentSubmesh.BigIndicesV.data(), NumIndices, 
				Remap.data());
			
			std::vector<float32> ResultVertices(TotalVertices * VertexStride);
			meshopt_remapVertexBuffer(ResultVertices.data(), 
				CurrentSubmesh.MeshVerticesV.data(), NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes(), Remap.data());
				
			CurrentSubmesh.MeshVerticesV = std::move(ResultVertices);
			CurrentSubmesh.BigIndicesV = std::move(ResultIndices);
			
			meshopt_optimizeVertexCache(CurrentSubmesh.GetLODIndices(0),
				CurrentSubmesh.GetLODIndices(0),
				CurrentSubmesh.GetLODNumOfIndices(0), 
				CurrentSubmesh.GetLODNumOfVertices(0));

			meshopt_optimizeVertexFetch(CurrentSubmesh.GetLODVertices(0), 
				CurrentSubmesh.GetLODIndices(0), 
				CurrentSubmesh.GetLODNumOfIndices(0), 
				CurrentSubmesh.GetLODVertices(0), 
				CurrentSubmesh.GetLODNumOfVertices(0), 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes());

			InMesh->m_AssetSize = TotalVertices * VertexStride * sizeof(float32);
			CurrentSubmesh.LODInfos[0].NumOfVertices = TotalVertices;
			CurrentSubmesh.LODInfos[0].VerticesOffset = 0;
			CurrentSubmesh.LODInfos[0].NumOfIndices = NumIndices;
			CurrentSubmesh.LODInfos[0].IndicesOffset = 0;

			NumVerticesAll += TotalVertices;
			NumIndicesAll += NumIndices;
		}

		InMesh->m_bAssetLoadedCPU = true;
		AssimpImporter.FreeScene();

		return true;
	}
	/*
	bool MeshLoader::LoadCube(Mesh* InMesh)
	{
		InMesh->m_SubmeshList.clear();

		uint32 NumVertices = 0;
		uint32 NumIndices = 0;

		uint32 NumVerticesAll = 0;
		uint32 NumIndicesAll = 0;

		for (uint32 SubmeshID = 0; SubmeshID < 1; SubmeshID++)
		{
			Submesh CurrentSubmesh;
			// Setup general info about submesh
			NumVertices = 8;
			NumIndices = 12 * 3;

			uint32 VertexStride = 0;

			// Position
			VertexStride += GetAttributeNumOfFloats(EAttribute::Position);

			float VertexData[] = {
				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};
			CurrentSubmesh.MeshVertices = reinterpret_cast<float*>(malloc(sizeof(VertexData)));
			memcpy(CurrentSubmesh.MeshVertices, VertexData, sizeof(VertexData));


			uint32 IndicesData[] = {
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
			CurrentSubmesh.BigIndicesData = reinterpret_cast<uint32*>(malloc(sizeof(IndicesData)));
			memcpy(CurrentSubmesh.BigIndicesData, IndicesData, sizeof(IndicesData));

			uint32* Remap = reinterpret_cast<uint32*>(malloc(NumIndices * sizeof(uint32)));

			uint64 TotalVertices = meshopt_generateVertexRemap(Remap, 
				CurrentSubmesh.BigIndicesData, NumIndices, 
				CurrentSubmesh.MeshVertices, NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes());
			
			uint32* ResultIndices = reinterpret_cast<uint32*>(malloc(NumIndices * sizeof(uint32)));
			meshopt_remapIndexBuffer(ResultIndices, 
				CurrentSubmesh.BigIndicesData, NumIndices, 
				Remap);
			
			float32* ResultVertices = reinterpret_cast<float*>(malloc(TotalVertices * VertexStride * sizeof(float32)));
			meshopt_remapVertexBuffer(ResultVertices, 
				CurrentSubmesh.MeshVertices, NumVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes(), Remap);

			free(CurrentSubmesh.MeshVertices);
			free(CurrentSubmesh.BigIndicesData);
			
			CurrentSubmesh.MeshVertices = ResultVertices;
			CurrentSubmesh.BigIndicesData = ResultIndices;
			

			InMesh->m_AssetSize = VertexStride * TotalVertices * sizeof(float32);
			//InMesh->m_AssetSize = VertexStride * NumVertices * sizeof(float32);
			CurrentSubmesh.NumOfVertices = TotalVertices;
			//CurrentSubmesh.NumOfVertices = NumVertices;
			CurrentSubmesh.NumOfIndices = NumIndices;

			NumVerticesAll += CurrentSubmesh.NumOfVertices;
			NumIndicesAll += CurrentSubmesh.NumOfIndices;

			InMesh->m_SubmeshList.push_back(std::move(CurrentSubmesh));
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

			meshopt_optimizeVertexCache(CurrentSubmesh.BigIndicesData, 
				CurrentSubmesh.BigIndicesData, 
				CurrentSubmesh.NumOfIndices, 
				CurrentSubmesh.NumOfVertices);

			meshopt_optimizeVertexFetch(CurrentSubmesh.MeshVertices, 
				CurrentSubmesh.BigIndicesData, 
				CurrentSubmesh.NumOfIndices, 
				CurrentSubmesh.MeshVertices, 
				CurrentSubmesh.NumOfVertices, 
				CurrentSubmesh.Attributes.GetVertexStrideInBytes());

			uint64 MaxMeshlets = meshopt_buildMeshletsBound(CurrentSubmesh.NumOfIndices, MaxVertices, MaxTriangles);

			std::vector<meshopt_Meshlet> Meshlets(MaxMeshlets);
			CurrentSubmesh.MeshletVertexIndices = reinterpret_cast<uint32*>(malloc(MaxMeshlets * MaxVertices * sizeof(uint32)));
			std::vector<uint8> MeshletTriangles(MaxMeshlets * MaxTriangles * 3);

			uint64 MeshletCount;
			if (CurrentSubmesh.bUseSmallIndices)
			{

			}
			else
			{
				MeshletCount = meshopt_buildMeshlets(Meshlets.data(), CurrentSubmesh.MeshletVertexIndices, MeshletTriangles.data(), 
					CurrentSubmesh.BigIndicesData,
					CurrentSubmesh.NumOfIndices,
					CurrentSubmesh.MeshVertices,
					CurrentSubmesh.NumOfVertices,
					CurrentSubmesh.Attributes.GetVertexStrideInBytes(),
					MaxVertices, MaxTriangles, ConeWeight);
			}

			// Trim arrays
			const meshopt_Meshlet& LastMeshlet = Meshlets[MeshletCount - 1];
			
			free(CurrentSubmesh.MeshletVertexIndices);
			CurrentSubmesh.MeshletVertexIndices = reinterpret_cast<uint32*>(malloc((LastMeshlet.vertex_offset + LastMeshlet.vertex_count) * sizeof(uint32)));
			MeshletTriangles.resize(LastMeshlet.triangle_offset + ((LastMeshlet.triangle_count * 3 + 3) & ~3));
			Meshlets.resize(MeshletCount);

			// Functionality added in newer library version
			// Further opimization per meshlet
			
			for (uint32 MeshletID = 0; MeshletID < MeshletCount; MeshletID++)
			{
				const meshopt_Meshlet& Meshlet = Meshlets[MeshletID]; 
				//meshopt_optimizeMeshlet(&MeshletVertices[Meshlet.vertex_offset], &MeshletTriangles[Meshlet.triangle_offset], Meshlet.triangle_count, Meshlet.vertex_count);
			}
			

			// Fit meshopt meshlets into internal formats
			CurrentSubmesh.MeshletTriangles = reinterpret_cast<Submesh::MeshletTriangle*>(malloc((MeshletTriangles.size() / 3) * sizeof(Submesh::MeshletTriangle)));
			CurrentSubmesh.Meshlets = reinterpret_cast<Submesh::Meshlet*>(malloc(Meshlets.size() * sizeof(Submesh::Meshlet)));

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
	*/
	void MeshLoader::BuildMeshletsWithCheck(Mesh* InMesh, bool OnlyLODs, bool Scan, bool Uniform)
	{
		// NVidia-recommends 64/126; we round 126 down to a multiple of 4
		// alternatively we also test uniform configuration with 64/64 which is better for AMD
		const uint64 MaxVertices = 64;
		const uint64 MaxTriangles = Uniform ? 64 : 124;

		// Note: should be set to 0 unless cone culling is used at runtime!
		const float ConeWeight = 0.0f;

		for (uint32 SubmeshID = 0; SubmeshID < InMesh->m_SubmeshList.size(); SubmeshID++)
		{
			Submesh& CurrentSubmesh = InMesh->m_SubmeshList[SubmeshID];
			
			for (uint32 LODIndex = (OnlyLODs) ? 1 : 0; LODIndex < CurrentSubmesh.LODInfos.size(); LODIndex++)
			{
				uint64 MaxMeshlets = meshopt_buildMeshletsBound(CurrentSubmesh.GetLODNumOfIndices(LODIndex), MaxVertices, MaxTriangles);

				std::vector<meshopt_Meshlet> Meshlets(MaxMeshlets);
				std::vector<uint32> MeshletVertexIndices(MaxMeshlets * MaxVertices);
				std::vector<uint8> MeshletTriangles(MaxMeshlets * MaxTriangles * 3);

				uint64 MeshletCount;
				if (Scan)
				{
					if (CurrentSubmesh.bUseSmallIndices)
					{

					}
					else
					{
						MeshletCount = meshopt_buildMeshletsScan(Meshlets.data(), MeshletVertexIndices.data(), MeshletTriangles.data(),
							CurrentSubmesh.GetLODIndices(LODIndex),
							CurrentSubmesh.GetLODNumOfIndices(LODIndex),
							CurrentSubmesh.GetLODNumOfVertices(LODIndex),
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
						MeshletCount = meshopt_buildMeshlets(Meshlets.data(), MeshletVertexIndices.data(), MeshletTriangles.data(),
							CurrentSubmesh.GetLODIndices(LODIndex),
							CurrentSubmesh.GetLODNumOfIndices(LODIndex),
							CurrentSubmesh.GetLODVertices(LODIndex),
							CurrentSubmesh.GetLODNumOfVertices(LODIndex),
							CurrentSubmesh.Attributes.GetVertexStrideInBytes(),
							MaxVertices, MaxTriangles, ConeWeight);
					}
				}
				Meshlets.resize(MeshletCount);

				for (uint64 i = 0; i < Meshlets.size(); i++)
				{
					meshopt_optimizeMeshlet(
						&MeshletVertexIndices[Meshlets[i].vertex_offset], 
						&MeshletTriangles[Meshlets[i].triangle_offset], 
						Meshlets[i].triangle_count, Meshlets[i].vertex_count);
				}

				if (Meshlets.size())
				{
					const meshopt_Meshlet& Last = Meshlets.back();

					// Trim the vertex/triangle arrays when copying data out to GPU storage
					MeshletVertexIndices.resize(Last.vertex_offset + Last.vertex_count);
					MeshletTriangles.resize(Last.triangle_offset + ((Last.triangle_count * 3 + 3) & ~3));
				}
			
				uint32 MeshletBoundsOffset = CurrentSubmesh.MeshletBoundsV.size();
				CurrentSubmesh.MeshletBoundsV.resize(CurrentSubmesh.MeshletBoundsV.size() + MeshletCount);
				for (size_t BoundsID = 0; BoundsID < Meshlets.size(); BoundsID++)
				{
					const meshopt_Meshlet& CurrentMeshlet = Meshlets[BoundsID];

					meshopt_Bounds Bounds = meshopt_computeMeshletBounds(
						&MeshletVertexIndices[CurrentMeshlet.vertex_offset], 
						&MeshletTriangles[CurrentMeshlet.triangle_offset], CurrentMeshlet.triangle_count, 
						CurrentSubmesh.MeshVerticesV.data(), CurrentSubmesh.GetLODNumOfVertices(LODIndex),
						CurrentSubmesh.Attributes.GetVertexStrideInBytes());

					uint32 GlobalID = MeshletBoundsOffset + BoundsID;
					CurrentSubmesh.MeshletBoundsV[GlobalID].Center[0] = Bounds.center[0];
					CurrentSubmesh.MeshletBoundsV[GlobalID].Center[1] = Bounds.center[1];
					CurrentSubmesh.MeshletBoundsV[GlobalID].Center[2] = Bounds.center[2];
					CurrentSubmesh.MeshletBoundsV[GlobalID].Radius = Bounds.radius;

					CurrentSubmesh.MeshletBoundsV[GlobalID].ConeApex[0] = Bounds.cone_apex[0];
					CurrentSubmesh.MeshletBoundsV[GlobalID].ConeApex[1] = Bounds.cone_apex[1];
					CurrentSubmesh.MeshletBoundsV[GlobalID].ConeApex[2] = Bounds.cone_apex[2];
					CurrentSubmesh.MeshletBoundsV[GlobalID].ConeAxis[0] = Bounds.cone_axis[0];
					CurrentSubmesh.MeshletBoundsV[GlobalID].ConeAxis[1] = Bounds.cone_axis[1];
					CurrentSubmesh.MeshletBoundsV[GlobalID].ConeAxis[2] = Bounds.cone_axis[2];
					CurrentSubmesh.MeshletBoundsV[GlobalID].ConeCutoff = Bounds.cone_cutoff;
				}

				// Fit meshopt meshlets into internal formats
				CurrentSubmesh.LODInfos[LODIndex].NumOfMeshlets = Meshlets.size();
				CurrentSubmesh.LODInfos[LODIndex].NumOfMeshletTriangles = MeshletTriangles.size() / 3;
				CurrentSubmesh.LODInfos[LODIndex].NumOfMeshletVertexIndices = MeshletVertexIndices.size();
				
				const uint32 MeshletsOffset = CurrentSubmesh.MeshletsV.size();
				const uint32 MeshletTrianglesOffset = CurrentSubmesh.MeshletTrianglesV.size();
				const uint32 MeshletVertexIndicesOffset = CurrentSubmesh.MeshletVertexIndicesV.size();

				CurrentSubmesh.LODInfos[LODIndex].MeshletsOffset = MeshletsOffset;
				CurrentSubmesh.LODInfos[LODIndex].MeshletTrianglesOffset = MeshletTrianglesOffset;
				CurrentSubmesh.LODInfos[LODIndex].MeshletVertexIndicesOffset = MeshletVertexIndicesOffset;
				
				CurrentSubmesh.MeshletsV.resize(MeshletsOffset + CurrentSubmesh.GetLODNumOfMeshlets(LODIndex));
				CurrentSubmesh.MeshletTrianglesV.resize(MeshletTrianglesOffset + CurrentSubmesh.GetLODNumOfMeshletTriangles(LODIndex));
				CurrentSubmesh.MeshletVertexIndicesV.resize(MeshletVertexIndicesOffset + CurrentSubmesh.GetLODNumOfMeshletVertexIndices(LODIndex));

				/*
				CurrentSubmesh.Meshlets = reinterpret_cast<Submesh::Meshlet*>(malloc(CurrentSubmesh.NumOfMeshlets * sizeof(Submesh::Meshlet)));
				CurrentSubmesh.MeshletTriangles = reinterpret_cast<Submesh::MeshletTriangle*>(malloc(CurrentSubmesh.NumOfMeshletTriangles * sizeof(Submesh::MeshletTriangle)));
				CurrentSubmesh.MeshletVertexIndices = reinterpret_cast<uint32*>(malloc(CurrentSubmesh.NumOfMeshletVertexIndices * sizeof(uint32)));
			
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
				*/
				
				memcpy(&CurrentSubmesh.MeshletVertexIndicesV[MeshletVertexIndicesOffset], MeshletVertexIndices.data(), MeshletVertexIndices.size() * sizeof(uint32));
					
				if (LODIndex == 0)
				for (uint32 MeshletID = 0; MeshletID < Meshlets.size(); MeshletID++)
				{
					const uint32 SpanEnd = Meshlets[MeshletID].triangle_offset + Meshlets[MeshletID].triangle_count * 3;
					for (uint32 TriangleID = Meshlets[MeshletID].triangle_offset; TriangleID < SpanEnd; TriangleID += 3)
					{
						Submesh::MeshletTriangle NewTriangle;
						NewTriangle.Index0 = MeshletTriangles[TriangleID + 0];
						NewTriangle.Index1 = MeshletTriangles[TriangleID + 1];
						NewTriangle.Index2 = MeshletTriangles[TriangleID + 2];

							CurrentSubmesh.MeshletTrianglesV[MeshletTrianglesOffset + TriangleID / 3] = NewTriangle;
					}
					
					uint32 GlobalMeshletID = MeshletsOffset + MeshletID;
					CurrentSubmesh.MeshletsV[GlobalMeshletID].VertexCount = Meshlets[MeshletID].vertex_count;
					CurrentSubmesh.MeshletsV[GlobalMeshletID].VertexOffset = Meshlets[MeshletID].vertex_offset;

					CurrentSubmesh.MeshletsV[GlobalMeshletID].TriangleCount = Meshlets[MeshletID].triangle_count;
					CurrentSubmesh.MeshletsV[GlobalMeshletID].TriangleOffset = Meshlets[MeshletID].triangle_offset / 3;

				}
			}
		}
	}

	void MeshLoader::AdjustMeshToVertexLayout(Mesh* InMesh, VertexLayout* NewVertexLayout)
	{
		
	}

	void MeshLoader::GenerateLODs(Mesh* InMesh, std::vector<Mesh::LODSetup>& LODSetups)
	{
		for (uint8 SubmeshID = 0; SubmeshID < InMesh->GetNumOfSubmeshes(); SubmeshID++)
		{
			Submesh& CurrentSubmesh = InMesh->GetSubmesh(SubmeshID);

			CurrentSubmesh.LODInfos.resize(1);
			CurrentSubmesh.BigIndicesV.resize(CurrentSubmesh.GetLODNumOfIndices(0));
			CurrentSubmesh.MeshVerticesV.resize(CurrentSubmesh.GetLODNumOfVertices(0));

			CurrentSubmesh.MeshletsV.resize(CurrentSubmesh.GetLODNumOfMeshlets(0));
			CurrentSubmesh.MeshletBoundsV.resize(CurrentSubmesh.GetLODNumOfMeshlets(0));
			CurrentSubmesh.MeshletTrianglesV.resize(CurrentSubmesh.GetLODNumOfMeshletTriangles(0));
			CurrentSubmesh.MeshletVertexIndicesV.resize(CurrentSubmesh.GetLODNumOfMeshletVertexIndices(0));
			
			for (uint8 LODSetupID = 0; LODSetupID < LODSetups.size(); LODSetupID++)
			{
				CurrentSubmesh.LODInfos.emplace_back();
				
				std::vector<uint32> NewIndices(CurrentSubmesh.GetLODNumOfIndices(0));

				float Error;
				uint64 NewSize = meshopt_simplify(
					NewIndices.data(), 
					CurrentSubmesh.BigIndicesV.data(), CurrentSubmesh.GetLODNumOfIndices(0),
					CurrentSubmesh.MeshVerticesV.data(), CurrentSubmesh.GetLODNumOfVertices(0),
					CurrentSubmesh.Attributes.GetVertexStrideInBytes(),
					LODSetups[LODSetupID].Percentage * CurrentSubmesh.GetLODNumOfIndices(0), 
					LODSetups[LODSetupID].Error, 0, &Error);
				
				uint32 Offset = CurrentSubmesh.BigIndicesV.size();
				CurrentSubmesh.BigIndicesV.resize(Offset + NewSize);
				memmove(&CurrentSubmesh.BigIndicesV[Offset], NewIndices.data(), NewSize * sizeof(uint32));

				Submesh::LODInfo& Info = CurrentSubmesh.LODInfos[LODSetupID];

				CurrentSubmesh.LODInfos.back().NumOfIndices = NewSize;
				CurrentSubmesh.LODInfos.back().IndicesOffset = Info.IndicesOffset + Info.NumOfIndices;

				CurrentSubmesh.LODInfos.back().NumOfVertices = CurrentSubmesh.GetLODNumOfVertices(0);
				CurrentSubmesh.LODInfos.back().VerticesOffset = 0;
			}
		}
	}
}