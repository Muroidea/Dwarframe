#ifndef MATERIALPROXY_H
#define MATERIALPROXY_H

#include "RootSignature.h"

namespace Dwarframe {
	
	class Shader;

	class MaterialProxy
	{
	public:
		MaterialProxy() = default;
		~MaterialProxy();

		void LoadAmplifierShader(std::string ShaderName);
		void LoadMeshShader(std::string ShaderName);
		void LoadPixelShader(std::string ShaderName);

		//void AddScalarParameter(std::string Name);
		//void Add2DTextureParameter(std::string Name);
		//void Add2DTextureParameter(std::string Name);

		RootSignature& GetRootSignatureRef() { return m_RootSignature; }
		class ID3D12RootSignature* GetRootSignature() { return m_RootSignature.GetRootSignature(); }

	private:
		RootSignature m_RootSignature;

		Shader* m_AmplifierShader = nullptr;
		Shader* m_MeshShader = nullptr;
		Shader* m_PixelShader = nullptr;

		template<typename T>
		friend class PipelineState;
	};

}

#endif // !MATERIALPROXY_H