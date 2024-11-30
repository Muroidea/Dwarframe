#ifndef SHADER_H
#define SHADER_H

namespace Dwarframe {

	enum EShaderType : int8
	{
		VertexShader = 0,
		HullShader,
		DomainShader,
		GeometryShader,
		PixelShader,
		AmplifierShader,
		MeshShader,
		ComputeShader,
		NumOfElements
	};

	inline const char* ShaderTypeToString(EShaderType Type)
	{
		switch (Type)
        {
			case VertexShader: return "VertexShader";
			case HullShader: return "HullShader";
			case DomainShader: return "DomainShader";
			case GeometryShader: return "GeometryShader";
			case PixelShader: return "PixelShader";
			case AmplifierShader: return "AmplifierShader";
			case MeshShader: return "MeshShader";
			case ComputeShader: return "ComputeShader";
			default: return "Undefined";
		}
	}
	
	inline const char* ShaderTypeToShortcut(EShaderType Type)
	{
		switch (Type)
        {
			case VertexShader: return "vs";
			case HullShader: return "hs";
			case DomainShader: return "ds";
			case GeometryShader: return "gs";
			case PixelShader: return "ps";
			case AmplifierShader: return "as";
			case MeshShader: return "ms";
			case ComputeShader: return "cs";
			default: return "";
		}
	}
	
	enum ERenderingPipelineType : uint8
	{
		ClassicPipeline,
		MeshShaderPipeline,
		ComputePipeline
	};

    inline void GetPipelineSupportedShaders(ERenderingPipelineType Pipeline, EShaderType*& ShaderList, uint32_t& NumOfShaders)
    {
        if (Pipeline == ERenderingPipelineType::ClassicPipeline)
        {
            ShaderList = new EShaderType[]{
                EShaderType::VertexShader,
                EShaderType::HullShader,
                EShaderType::DomainShader,
                EShaderType::GeometryShader,
                EShaderType::PixelShader
            };

            NumOfShaders = 5;
        }
        else if (Pipeline == ERenderingPipelineType::MeshShaderPipeline)
        {
            ShaderList = new EShaderType[]{
                EShaderType::AmplifierShader,
                EShaderType::MeshShader,
                EShaderType::PixelShader
            };

            NumOfShaders = 3;
        }
        else // Compute shader
        {
            ShaderList = new EShaderType[]{
                EShaderType::ComputeShader
            };
            
            NumOfShaders = 1;
        }
    }

    inline bool IsShaderRequired(ERenderingPipelineType Pipeline, EShaderType ShaderList)
    {
        if (Pipeline == ERenderingPipelineType::ClassicPipeline)
        {
            switch (ShaderList)
            {
                case EShaderType::VertexShader:
                case EShaderType::PixelShader:
                    return true;
                default:
                    return false;
            }
        }
        else if (Pipeline == ERenderingPipelineType::MeshShaderPipeline)
        {
            switch (ShaderList)
            {
                case EShaderType::MeshShader:
                case EShaderType::PixelShader:
                    return true;
                default:
                    return false;
            }
        }
        else // Compute pipeline
        {
            if (EShaderType::ComputeShader) return true;
            else return false;
        }
    }

	class Shader
	{
	public:
		Shader(std::string_view ShaderName);
		~Shader();

		inline void SetShaderType(EShaderType NewType) { m_ShaderType = NewType; }
		inline std::string& GetPath() { return m_Path; }
		inline std::string& GetEntryPoint() { return m_EntryPoint; }

		inline EShaderType GetShaderType() const { return m_ShaderType; }
		inline const void* GetShaderBinary() const { return m_Data; }
		inline size_t GetShaderSize() const { return m_Size; }
		inline bool IsCompiled() const { return m_Compiled; }

	private:
	
		enum class EResourceType : uint8
		{ 
			DirectConstant,
			ConstantBufferView,
			UnorderedAccessViewUntyped,
			ShaderResourceViewUntyped,
			UnorderedAccessView,
			ShaderResourceView,
			Sampler
		};
		
		enum class EResourceRequiredScope : uint8
		{ 
			RootConstants,
			RootDescriptor,
			RootDescriptorTable
		};

		struct BoundedResource
		{
			// Information acquired from reflection
			std::string_view m_Name;
			D3D12_SHADER_VISIBILITY m_Visibility;
			uint8 m_BindPoint {};
			uint8 m_Space {};
			EResourceType m_Type {};

			// Assigned during RootSignature creation
			//CRootParam mRP;
		};

		EShaderType m_ShaderType;
		void* m_Data = nullptr;
		size_t m_Size = 0;
		std::string m_Path;
		std::string m_EntryPoint;
		
		bool m_ResourceDescriptorsHeapIndexing : 1;
		bool m_SamplerDescriptorsHeapIndexing : 1;
		bool m_Compiled : 1;

        ComPtr<ID3D12ShaderReflection> m_Reflection;
		std::vector<D3D12_SHADER_INPUT_BIND_DESC> m_BoundData;
		std::vector<BoundedResource> m_BoundedResources;

		friend class ShaderCompiler;
        friend class Material;
	};

}

#endif // ! SHADER_H

