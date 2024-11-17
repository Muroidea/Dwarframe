#include "pch.h"
#include "SimpleMeshGenerator.h"

namespace Dwarframe {

	// Tangents, UV, Normals, Indices and Vertices
	
	SimpleMesh SimpleMeshGenerator::GenerateCubeSphere()
	{
		SimpleMesh NewMesh;

		return NewMesh;
	}
	
	SimpleMesh SimpleMeshGenerator::GenerateCube(float32 Width, float32 Height, float32 Depth)
	{
		SimpleMesh NewMesh;
		NewMesh.AdjustSize(24, 36);

		Width = 0.5f * Width;
		Height = 0.5f * Height;
		Depth = 0.5f * Depth;
    
		// Fill in the front face vertex data.
		NewMesh.AddVertex({-Width, -Height, -Depth}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({-Width,  Height, -Depth}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ Width,  Height, -Depth}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ Width, -Height, -Depth}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f});

		// Fill in the back face vertex data.
		NewMesh.AddVertex({-Width, -Height,  Depth}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ Width, -Height,  Depth}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ Width,  Height,  Depth}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({-Width,  Height,  Depth}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f});

		// Fill in the top face vertex data.
		NewMesh.AddVertex({-Width,  Height, -Depth}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({-Width,  Height,  Depth}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ Width,  Height,  Depth}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ Width,  Height, -Depth}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f});

		// Fill in the bottom face vertex data.
		NewMesh.AddVertex({-Width, -Height, -Depth}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ Width, -Height, -Depth}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ Width, -Height,  Depth}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({-Width, -Height,  Depth}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f});

		// Fill in the left face vertex data.
		NewMesh.AddVertex({-Width, -Height,  Depth}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f});
		NewMesh.AddVertex({-Width,  Height,  Depth}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f});
		NewMesh.AddVertex({-Width,  Height, -Depth}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f});
		NewMesh.AddVertex({-Width, -Height, -Depth}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f});

		// Fill in the right face vertex data.
		NewMesh.AddVertex({ Width, -Height, -Depth}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
		NewMesh.AddVertex({ Width,  Height, -Depth}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
		NewMesh.AddVertex({ Width,  Height,  Depth}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
		NewMesh.AddVertex({ Width, -Height,  Depth}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f});
		
		// Fill in the front face index data
		NewMesh.AddTriangle(0, 1, 2);
		NewMesh.AddTriangle(0, 2, 3);

		// Fill in the back face index data
		NewMesh.AddTriangle(4, 5, 6);
		NewMesh.AddTriangle(4, 6, 7);

		// Fill in the top face index data
		NewMesh.AddTriangle(8, 9, 10);
		NewMesh.AddTriangle(8, 10, 11);

		// Fill in the bottom face index data
		NewMesh.AddTriangle(12, 13, 14);
		NewMesh.AddTriangle(12, 14, 15);

		// Fill in the left face index data
		NewMesh.AddTriangle(16, 17, 18);
		NewMesh.AddTriangle(16, 18, 19);

		// Fill in the right face index data
		NewMesh.AddTriangle(20, 21, 22);
		NewMesh.AddTriangle(20, 22, 23);

		return NewMesh;
	}

	SimpleMesh SimpleMeshGenerator::GenerateSimpleCube(float32 Width, float32 Height, float32 Depth)
	{
		SimpleMesh NewMesh;
		NewMesh.AdjustSize(8, 36);

		// Fill in the front face vertex data.
		NewMesh.AddVertex({-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f});

		// Fill in the back face vertex data.
		NewMesh.AddVertex({-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f});
		NewMesh.AddVertex({ 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f});

		// Top face
		NewMesh.AddTriangle(0, 1, 2);
		NewMesh.AddTriangle(0, 2, 3);

		// Bottom face
		NewMesh.AddTriangle(4, 6, 5);
		NewMesh.AddTriangle(4, 7, 6);

		// Left face
		NewMesh.AddTriangle(4, 5, 1);
		NewMesh.AddTriangle(4, 1, 0);

		// Right face
		NewMesh.AddTriangle(3, 2, 6);
		NewMesh.AddTriangle(3, 6, 7);

		// Front face
		NewMesh.AddTriangle(1, 5, 6);
		NewMesh.AddTriangle(1, 6, 2);

		// Back face
		NewMesh.AddTriangle(4, 0, 3);
		NewMesh.AddTriangle(4, 3, 7);

		return NewMesh;
	}

	SimpleMesh SimpleMeshGenerator::GenerateCube3()
	{
		return SimpleMesh();
	}

	SimpleMesh SimpleMeshGenerator::GenerateUVSphere(uint32 NumOfSegmentsU, uint32 NumOfSegmentsV, float32 Radius)
	{
		return SimpleMesh();
	}



}

