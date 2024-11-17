#include "pch.h"
#include "RootSignature.h"

#include "Dwarframe/Renderer/Pipeline/DescriptorTable.h"

#ifdef _WIN32
void PrintMessageForHresult(HRESULT HResult)
{
	_com_error Error(HResult);
	printf("Error 0x%08x: %s", HResult, Error.ErrorMessage());
}
#endif

namespace Dwarframe {

	RootSignature::RootSignature()
	{
                
	}
    
	RootSignature::~RootSignature()
	{
		m_RootSignature.Reset();
        m_RootSignature = nullptr;
	}
    
	void RootSignature::Release()
	{
		if (m_RootSignature)
		{
			m_RootSignature.Reset();
			m_RootSignature = nullptr;
		}

		m_BindingMap.clear();
		m_Parameters.clear();
		m_DescriptorRange.clear();
		m_StaticSamplers.clear();
		m_NumOfRangesPerDescriptorTable.clear();
	}

	void RootSignature::AddConstantParameter(uint32 Num32BitValues, uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace)
	{
		D3D12_ROOT_PARAMETER1 RootParameter{};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		RootParameter.Constants.ShaderRegister = ShaderRegister;
		RootParameter.Constants.RegisterSpace = RegisterSpace;
		RootParameter.Constants.Num32BitValues = Num32BitValues;
		RootParameter.ShaderVisibility = ShaderVisibility;

		//RootParametersSize += 4;

		m_Parameters.push_back(std::move(RootParameter));
    }
    
    void RootSignature::AddSRVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
    {
        D3D12_ROOT_PARAMETER1 RootParameter {};
        RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.Descriptor.Flags = Flags;
        RootParameter.ShaderVisibility = ShaderVisibility;

		//RootParametersSize += 8;

        m_Parameters.push_back(std::move(RootParameter));
    }

    void RootSignature::AddUAVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
    {
		D3D12_ROOT_PARAMETER1 RootParameter{};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.Descriptor.Flags = Flags;
		RootParameter.ShaderVisibility = ShaderVisibility;

		//RootParametersSize += 8;

		m_Parameters.push_back(std::move(RootParameter));
    }

    void RootSignature::AddCBVParameter(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace, D3D12_ROOT_DESCRIPTOR_FLAGS Flags)
    {
		D3D12_ROOT_PARAMETER1 RootParameter{};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		RootParameter.Descriptor.ShaderRegister = ShaderRegister;
		RootParameter.Descriptor.RegisterSpace = RegisterSpace;
		RootParameter.Descriptor.Flags = Flags;
		RootParameter.ShaderVisibility = ShaderVisibility;

		//RootParametersSize += 8;

		m_Parameters.push_back(std::move(RootParameter));
    }

    void RootSignature::AddStaticSampler(uint32 ShaderRegister, D3D12_SHADER_VISIBILITY ShaderVisibility, uint32 RegisterSpace)
    {
		D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc{};
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
		StaticSamplerDesc.ShaderRegister = ShaderRegister;
		StaticSamplerDesc.RegisterSpace = RegisterSpace;
		StaticSamplerDesc.ShaderVisibility = ShaderVisibility;

		m_StaticSamplers.push_back(std::move(StaticSamplerDesc));
    }

    void RootSignature::BeginDescriptorTableParameter(D3D12_SHADER_VISIBILITY ShaderVisibility)
    {
		D3D12_ROOT_PARAMETER1 RootParameter{};
		RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		RootParameter.ShaderVisibility = ShaderVisibility;

		//RootParametersSize += 4;

		m_Parameters.push_back(std::move(RootParameter));
    }

    void RootSignature::AddSRVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags, uint32 RegisterSpace)
    {
		D3D12_DESCRIPTOR_RANGE1 Range{};
		Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		Range.NumDescriptors = NumDescriptors;
		Range.BaseShaderRegister = BaseShaderRegister;
		Range.RegisterSpace = RegisterSpace;
		Range.Flags = Flags;
		Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		m_DescriptorRange.push_back(std::move(Range));
    }

    void RootSignature::AddUAVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags, uint32 RegisterSpace)
    {
		D3D12_DESCRIPTOR_RANGE1 Range{};
		Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		Range.NumDescriptors = NumDescriptors;
		Range.BaseShaderRegister = BaseShaderRegister;
		Range.RegisterSpace = RegisterSpace;
		Range.Flags = Flags;
		Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		m_DescriptorRange.push_back(std::move(Range));
    }

    void RootSignature::AddCBVRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags, uint32 RegisterSpace)
    {
		D3D12_DESCRIPTOR_RANGE1 Range{};
		Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		Range.NumDescriptors = NumDescriptors;
		Range.BaseShaderRegister = BaseShaderRegister;
		Range.RegisterSpace = RegisterSpace;
		Range.Flags = Flags;
		Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		m_DescriptorRange.push_back(std::move(Range));
    }

    void RootSignature::AddSamplerRange(uint32 NumDescriptors, uint32 BaseShaderRegister, D3D12_DESCRIPTOR_RANGE_FLAGS Flags, uint32 RegisterSpace)
	{
		D3D12_DESCRIPTOR_RANGE1 Range{};
		Range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		Range.NumDescriptors = NumDescriptors;
		Range.BaseShaderRegister = BaseShaderRegister;
		Range.RegisterSpace = RegisterSpace;
		Range.Flags = Flags;
		Range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		m_DescriptorRange.push_back(std::move(Range));
    }

    void RootSignature::EndDescriptorTableParameter()
    {
        if (m_NumOfRangesPerDescriptorTable.size() == 0)
        {
            m_NumOfRangesPerDescriptorTable.push_back(m_DescriptorRange.size());
            return;
        }
        
         m_NumOfRangesPerDescriptorTable.push_back(m_DescriptorRange.size() - *m_NumOfRangesPerDescriptorTable.end());
    }

    bool RootSignature::EndRootSignature(ID3D12Device8* D3D12Device, D3D12_ROOT_SIGNATURE_FLAGS Flags)
    {
		uint32 DescriptorTableIndex = 0;
		uint32 RangeOffset = 0;
		for (uint32 Index = 0; Index < m_Parameters.size(); Index++)
		{
			if (m_Parameters[Index].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				m_Parameters[Index].DescriptorTable.NumDescriptorRanges = m_NumOfRangesPerDescriptorTable[DescriptorTableIndex];
				m_Parameters[Index].DescriptorTable.pDescriptorRanges = m_DescriptorRange.data() + RangeOffset;
				RangeOffset += m_NumOfRangesPerDescriptorTable[DescriptorTableIndex];
				DescriptorTableIndex++;
			}
		}

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC VersionedRootSignatureDesc {};
        VersionedRootSignatureDesc.Version = m_RootSignatureVersion;

		D3D12_ROOT_SIGNATURE_DESC1 RootDescriptorTable1 {};
		RootDescriptorTable1.NumParameters = m_Parameters.size();
		RootDescriptorTable1.pParameters = m_Parameters.data();
		RootDescriptorTable1.NumStaticSamplers = m_StaticSamplers.size();
		RootDescriptorTable1.pStaticSamplers = m_StaticSamplers.data();
		RootDescriptorTable1.Flags = Flags;

		VersionedRootSignatureDesc.Desc_1_1 = RootDescriptorTable1;
        

        ID3DBlob *RootSignatureBlob = nullptr, *ErrorBlob = nullptr;
        HRESULT HResult = D3D12SerializeVersionedRootSignature(&VersionedRootSignatureDesc, &RootSignatureBlob, &ErrorBlob);
        if (FAILED(HResult))
        {
            if (ErrorBlob && ErrorBlob->GetBufferSize() > 0)
            {
                const char* ErrorMessage = (char*)ErrorBlob->GetBufferPointer();
                printf(ErrorMessage);
            }

			return false;
        }
        
		HResult = D3D12Device->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)); 
		if (FAILED(HResult))
		{
			PrintMessageForHresult(HResult);
			if (m_RootSignature)
			{
				m_RootSignature->Release();
			}
			return false;
		}

		m_Parameters.clear();
		m_DescriptorRange.clear();
		m_StaticSamplers.clear();
		m_NumOfRangesPerDescriptorTable.clear();
		return true;
    }
}