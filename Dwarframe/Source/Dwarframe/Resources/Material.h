#ifndef MATERIAL_H
#define MATERIAL_H

#include "GameAsset.h"
#include "Dwarframe/Renderer/GPUResources/Shader.h"
#include "Dwarframe/Renderer/Pipeline/RootSignature.h"

namespace Dwarframe {
    
    class Material : public GameAsset
    {
    IMPLEMENT_SIMPLE_RTTI(Material)

    public:
        Material(std::string AssetPath, std::string AssetName);

        virtual bool LoadAsset() override;
		virtual bool SaveAsset() override;

		const void* GetShaderBinary(EShaderType ShaderType) const;
		uint64 GetShaderSize(EShaderType ShaderType) const;

        ERenderingPipelineType GetRenderingPipelineType() const { return m_Pipeline; }
        void SetRenderingPipelineType(ERenderingPipelineType NewPipeline);

		//const RootSignature& GetRootSignatureRef() const { return m_RootSignature; }
		class ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.GetRootSignature(); }

#if WITH_EDITOR
		virtual void RenderProperties() override;
#endif
    private:
        ERenderingPipelineType m_Pipeline;
		std::vector<Shader*> m_ShaderList;

		RootSignature m_RootSignature;

        friend class MaterialManager;
    };

}

#endif // !MATERIAL_H