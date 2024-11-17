#ifndef ROOTSIGNATURE_H
#define ROOTSIGNATURE_H

namespace Dwarframe {
	
	class RootSignature
	{
	public:
		
		RootSignature();
		~RootSignature();

		inline bool IsValid() { return m_RootSignature.Get(); }

		void Release();
		inline ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }
		
		inline void SetBindingMap(std::map<HashValue, uint32>&& Map) { m_BindingMap = Map;}
		inline uint32 GetBinding(std::string_view Name) { return m_BindingMap[std::hash<std::string_view>{}(Name)]; }

		void AddConstantParameter(uint32 Num32BitValues, uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0);
		void AddSRVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
		void AddUAVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
		void AddCBVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
		void AddStaticSampler(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0);

		void BeginDescriptorTableParameter(D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL);

		void AddSRVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32 RegisterSpace = 0); // Adding range to lastly added descriptor table
		void AddUAVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32 RegisterSpace = 0);
		void AddCBVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32 RegisterSpace = 0);
		void AddSamplerRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32 RegisterSpace = 0);
		
		void EndDescriptorTableParameter();

		bool EndRootSignature(ID3D12Device8* D3D12Device, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		
	private:
		std::map<HashValue, uint32> m_BindingMap;

		D3D_ROOT_SIGNATURE_VERSION m_RootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		ComPtr<ID3D12RootSignature> m_RootSignature;
		
		std::vector<D3D12_ROOT_PARAMETER1> m_Parameters;
		std::vector<D3D12_STATIC_SAMPLER_DESC> m_StaticSamplers;
		std::vector<D3D12_DESCRIPTOR_RANGE1> m_DescriptorRange;
		//std::vector<DescriptorTable*> m_DescriptorHeaps;

		std::vector<uint32> m_NumOfRangesPerDescriptorTable;
		
		// TODO: Add automatic counting of parameters types
		// It should be done somewhere else - before RootSignature is created
		// We cannot do much here if we exceed size or it would be overcomplicated
		//uint32 RootParametersSize;	// The size of all root parameters in the root signature. Size in DWORDs, the limit is 64.
		
	};
	/*
	template <D3D_ROOT_SIGNATURE_VERSION Version>
	class TRootSignature
	{
	public:
		using RootParameterType = std::conditional<Version <= D3D_ROOT_SIGNATURE_VERSION_1_0, D3D12_ROOT_PARAMETER, D3D12_ROOT_PARAMETER1>::type;
		using StaticSamplerType = std::conditional<Version <= D3D_ROOT_SIGNATURE_VERSION_1_1, D3D12_STATIC_SAMPLER_DESC, D3D12_STATIC_SAMPLER_DESC1> ::type;
		using DescriptorRangeType = std::conditional<Version <= D3D_ROOT_SIGNATURE_VERSION_1_0, D3D12_DESCRIPTOR_RANGE, D3D12_DESCRIPTOR_RANGE1>::type;

		TRootSignature();
		~TRootSignature();

		inline void ReleaseRootSignature() { m_RootSignature.Reset(); }
		inline ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

		void AddConstantParameter(uint32 Num32BitValues, uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0);

		void AddSRVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
		void AddUAVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
		void AddCBVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL, uint32 RegisterSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
		
		void AddStaticSampler();
		
		void AddDescriptorTableParameter(D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL);

		void BeginDescriptorTableParameter(D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL);

		void AddSRVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32 RegisterSpace = 0); // Adding range to lastly added descriptor table
		void AddUAVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32 RegisterSpace = 0);
		void AddCBVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32 RegisterSpace = 0);
		void AddSamplerRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32 RegisterSpace = 0);

		void EndDescriptorTableParameter();

		void EndRootSignature(ID3D12Device8* D3D12Device, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		
	private:
		D3D_ROOT_SIGNATURE_VERSION m_RootSignatureVersion = Version;
		ComPtr<ID3D12RootSignature> m_RootSignature;

		std::vector<RootParameterType> m_Parameters;
		std::vector<StaticSamplerType> m_StaticSamplers;
		std::vector<DescriptorRangeType> m_DescriptorRange;
		//std::vector<DescriptorTable*> m_DescriptorHeaps;

		std::vector<uint32> m_NumOfRangesPerDescriptorTable;

		uint32 RootParametersSize; // The size of all root parameters in the root signature. Size in DWORDs, the limit is 64.
	};
	

	template <>
	class TRootSignature<D3D_ROOT_SIGNATURE_VERSION_1_0>
	{
		
	};

	template <D3D_ROOT_SIGNATURE_VERSION Version>
	void TRootSignature<Version>::AddStaticSampler()
	{
		StaticSamplerType StaticSamplerDesc{};
		StaticSamplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		StaticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		StaticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		StaticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		StaticSamplerDesc.MipLODBias = 0;
		StaticSamplerDesc.MaxAnisotropy = 16;
		StaticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		StaticSamplerDesc.MinLOD = 0.0f;
		StaticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		StaticSamplerDesc.ShaderRegister = 0;
		StaticSamplerDesc.RegisterSpace = 0;
		StaticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		m_StaticSamplers.push_back(std::move(StaticSamplerDesc));
	}

	template <>
	void TRootSignature<D3D_ROOT_SIGNATURE_VERSION_1_0>::AddCBVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
	{
		D3D12_ROOT_PARAMETER RootParameter {};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.ShaderVisibility = ShaderVisibility;

		m_Parameters.push_back(std::move(RootParameter));
	}

	template <>
	void TRootSignature<D3D_ROOT_SIGNATURE_VERSION_1_1>::AddCBVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
	{
		D3D12_ROOT_PARAMETER1 RootParameter {};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.Descriptor.Flags = Flags;
		RootParameter.ShaderVisibility = ShaderVisibility;

		m_Parameters.push_back(std::move(RootParameter));
	}

	template <>
	void TRootSignature<D3D_ROOT_SIGNATURE_VERSION_1_0>::AddUAVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
	{
		D3D12_ROOT_PARAMETER RootParameter {};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.ShaderVisibility = ShaderVisibility;

		m_Parameters.push_back(std::move(RootParameter));
	}

	template <>
	void TRootSignature<D3D_ROOT_SIGNATURE_VERSION_1_1>::AddUAVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
	{
		D3D12_ROOT_PARAMETER1 RootParameter {};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.Descriptor.Flags = Flags;
		RootParameter.ShaderVisibility = ShaderVisibility;

		m_Parameters.push_back(std::move(RootParameter));
	}

	template <>
	void TRootSignature<D3D_ROOT_SIGNATURE_VERSION_1_0>::AddSRVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
	{
		D3D12_ROOT_PARAMETER RootParameter {};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.ShaderVisibility = ShaderVisibility;

		m_Parameters.push_back(std::move(RootParameter));
	}

	template <>
	void TRootSignature<D3D_ROOT_SIGNATURE_VERSION_1_1>::AddSRVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
	{
		D3D12_ROOT_PARAMETER1 RootParameter {};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.Descriptor.Flags = Flags;
		RootParameter.ShaderVisibility = ShaderVisibility;

		m_Parameters.push_back(std::move(RootParameter));
	}

	template <D3D_ROOT_SIGNATURE_VERSION Version>
	void TRootSignature<Version>::AddConstantParameter(uint32 Num32BitValues, uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace)
	{
		D3D12_ROOT_PARAMETER RootParameter {};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.ShaderVisibility = ShaderVisibility;

		m_Parameters.push_back(std::move(RootParameter));
	}

	template <D3D_ROOT_SIGNATURE_VERSION Version>
	TRootSignature<Version>::TRootSignature()
	{

	}

	template <D3D_ROOT_SIGNATURE_VERSION Version>
	Dwarframe::TRootSignature<Version>::~TRootSignature()
	{

	}*/
}

#endif