#ifndef PCH_H
#define PCH_H

#include <algorithm>
#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef _WIN32

	#include <DirectXMath.h>
	#include <d3d12.h>
	#include <d3d12shader.h>
	#include <dxgi1_6.h>
	#include <DirectXMesh.h>

	#ifdef _DEBUG
		#include <d3d12sdklayers.h>
		#include <dxgidebug.h>
	#endif

	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	#define NOMINMAX
	// Windows Header Files
	#include <windows.h>
	#include <wincodec.h>
	#include <wrl.h>
	#include <comdef.h>

	using namespace Microsoft::WRL;
	using namespace DirectX;

#endif

//***** Framework ******
#include "Dwarframe/Core.h"
#include "Dwarframe/Helpers/Paths.h"
#include "Dwarframe/Helpers/Alignments.h"
#include "Dwarframe/Helpers/MeshLoader.h"
#include "Dwarframe/Helpers/TextureLoader.h"
#include "Dwarframe/Math/Transform.h"

#if WITH_EDITOR
#include "Dwarframe/Editor/Editor.h"
#endif
#include "Dwarframe/Editor/EditorExtender.h"

#endif // !PCH_H



