#ifndef SIMPLEMESHGENERATOR_H
#define SIMPLEMESHGENERATOR_H

namespace Dwarframe {

	struct SimpleMesh
	{
		inline void AdjustSize(uint32 NumOfVertices, uint32 NumOfIndices)
		{
			Positions.reserve(NumOfVertices);
			Normals.reserve(NumOfVertices);
			UVs.reserve(NumOfVertices);
			TangentUs.reserve(NumOfVertices);

			Indices.reserve(NumOfIndices);
		}

		inline void AddVertex(XMFLOAT3 Position, XMFLOAT3 Normal, XMFLOAT2 UV, XMFLOAT3 TangentU)
		{
			Positions.push_back(Position);
			Normals.push_back(Normal);
			UVs.push_back(UV);
			TangentUs.push_back(TangentU);
		}

		inline void AddTriangle(uint32 Index1, uint32 Index2, uint32 Index3)
		{
			Indices.push_back(Index1);
			Indices.push_back(Index2);
			Indices.push_back(Index3);
		}

		inline void* GetPositionsData()		{ return Positions.data(); }
		inline void* GetNormalsData()		{ return Normals.data(); }
		inline void* GetUVsData()			{ return UVs.data(); }
		inline void* GetTangentUsData()		{ return TangentUs.data(); }
		inline void* GetIndicesData()		{ return Indices.data(); }

		inline uint64 GetPositionsSize() const		{ return Positions.size() * sizeof(XMFLOAT3); }
		inline uint64 GetNormalsSize() const		{ return Normals.size() * sizeof(XMFLOAT3); }
		inline uint64 GetUVsSize() const	 		{ return UVs.size() * sizeof(XMFLOAT2); }
		inline uint64 GetTangaentUsSize() const		{ return TangentUs.size() * sizeof(XMFLOAT3); }
		inline uint64 GetIndicesSize() const		{ return Indices.size() * sizeof(uint32); }

		inline uint64 GetSizeInBytes() const		{ return GetPositionsSize() + GetNormalsSize() + GetUVsSize() + GetTangaentUsSize() + GetIndicesSize(); }

		std::vector<XMFLOAT3> Positions;
		std::vector<XMFLOAT3> Normals;
		std::vector<XMFLOAT2> UVs;
		std::vector<XMFLOAT3> TangentUs;

		std::vector<uint32> Indices;
	};

	class SimpleMeshGenerator
	{
	public:
		static SimpleMesh GenerateUVSphere(uint32 NumOfSectors, uint32 NumOfSegments, float32 Radius = 1.0f);
		static SimpleMesh GenerateCube(float32 Width, float32 Height, float32 Depth);
		static SimpleMesh GenerateSimpleCube(float32 Width, float32 Height, float32 Depth);
		static SimpleMesh GenerateCube3();
		static SimpleMesh GenerateCubeSphere();

	private:
		SimpleMeshGenerator() = default;
		SimpleMeshGenerator(const SimpleMeshGenerator&) = default;
		SimpleMeshGenerator& operator=(const SimpleMeshGenerator&) = default;
	};

}

#endif // !SIMPLEMESHGENERATOR_H

