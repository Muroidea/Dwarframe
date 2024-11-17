#include "pch.h"
#include "ShaderCompiler.h"

#include "Dwarframe/Renderer/HardwareBridge/GraphicsBridge.h"

#include "Dwarframe/Renderer/Pipeline/RootSignature.h"
#include "Dwarframe/Renderer/DXContext.h"

#include <dxcapi.h>

namespace Dwarframe {
/*
    class CustomIncludeHandler : public IDxcIncludeHandler
    {
    public:
        HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR Filename, _COM_Outptr_result_maybenull_ IDxcBlob** IncludeSource) override
        {
            ComPtr<IDxcBlobEncoding> pEncoding;
            std::string path = Paths::Normalize(UNICODE_TO_MULTIBYTE(Filename));
            if (m_IncludedFiles.find(path) != m_IncludedFiles.end())
            {
                // Return empty string blob if this file has been included before
                static const char nullStr[] = " ";
                pUtils->CreateBlobFromPinned(nullStr, ARRAYSIZE(nullStr), DXC_CP_ACP, pEncoding.GetAddressOf());
                *IncludeSource = pEncoding.Detach();
                return S_OK;
            }

            HRESULT hr = pUtils->LoadFile(Filename, nullptr, pEncoding.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                m_IncludedFiles.insert(path);
                *IncludeSource = pEncoding.Detach();
            }
            return hr;
        }

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override { return E_NOINTERFACE; }
        ULONG STDMETHODCALLTYPE AddRef(void) override {	return 0; }
        ULONG STDMETHODCALLTYPE Release(void) override { return 0; }

        std::unordered_set<std::string> m_IncludedFiles;
    };
    */
	bool ShaderCompiler::Load(Shader* InShader)
	{
        std::filesystem::path ShaderPath { InShader->GetPath() };
        
        if (!std::filesystem::exists(ShaderPath))
        {
            return false;
        }

        ComPtr<IDxcBlob> ShaderBlob; 
        std::wstring EntryPointName; 
        std::wstring ShaderModel;

		ComPtr<IDxcLibrary> Library;
        ThrowIfFailed(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&Library)));
    
        ComPtr<IDxcCompiler> Compiler;
        ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler)));

        uint32 CodePage = CP_UTF8;
        ComPtr<IDxcBlobEncoding> SourceBlob;
        ThrowIfFailed(Library->CreateBlobFromFile(ShaderPath.c_str(), &CodePage, &SourceBlob));

        ComPtr<IDxcOperationResult> Result;
        HRESULT HR = Compiler->Compile(SourceBlob.Get(), ShaderPath.c_str(), EntryPointName.c_str(),
            ShaderModel.c_str(), NULL, 0, NULL, 0, NULL, &Result);
    
        if (SUCCEEDED(HR))
        {
            Result->GetStatus(&HR);
        }

        if (FAILED(HR))
        {
            if (Result)
            {
                ComPtr<IDxcBlobEncoding> ErrorsBlob;
                HR = Result->GetErrorBuffer(&ErrorsBlob);
                if (SUCCEEDED(HR) && ErrorsBlob)
                {
    #if defined(_DEBUG)
                    // Print new size
                    char Buffer[500];
                    wchar_t wBuffer[500];
                    sprintf_s(Buffer, 500, "Compilation failed with errors:\n%hs\n", (const char*)ErrorsBlob->GetBufferPointer());
                    uint64 OutSize;
                    mbstowcs_s(&OutSize, wBuffer, Buffer, strlen(Buffer) + 1);
                    OutputDebugString(wBuffer);;    
    #endif
                }
            }
        }

        Result->GetResult(&ShaderBlob);

		return true;
	}

    bool ShaderCompiler::Compile(Shader* InShader)
    {
        std::filesystem::path ShaderPath { InShader->GetPath() };

        if (!std::filesystem::exists(ShaderPath))
        {
            return false;
        }

        ComPtr<IDxcUtils> Utils;
        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(Utils.ReleaseAndGetAddressOf()));
        
        ComPtr<IDxcIncludeHandler> IncludeHandler;
        Utils->CreateDefaultIncludeHandler(IncludeHandler.ReleaseAndGetAddressOf());
        
        ComPtr<IDxcBlobEncoding> SourceBlob;
        uint32 CodePage = CP_UTF8;
        Utils->LoadFile(ShaderPath.c_str(), &CodePage, SourceBlob.GetAddressOf());

        std::vector<LPCWSTR> Arguments;
        // -E for the entry point (eg. 'main')
        Arguments.emplace_back(L"-E");
        Arguments.emplace_back(L"main");

        // -T for the target profile (eg. 'ps_6_6')
        Arguments.emplace_back(L"-T");
        std::string Model;
        Model.append(ShaderTypeToShortcut(InShader->GetShaderType()));
        Model.append("_"); 
        Model.append(ShaderModelToShortcut(GraphicsBridge::Get().GetShaderModel()));
        std::wstring wsTmp(Model.begin(), Model.end());
        //Arguments.emplace_back(L"vs_6_6");
        
        Arguments.emplace_back(wsTmp.c_str());

        // Strip reflection data and pdbs (see later)
        Arguments.emplace_back(L"-Qstrip_debug");
        Arguments.emplace_back(L"-Qstrip_reflect");

        Arguments.emplace_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
        Arguments.emplace_back(DXC_ARG_DEBUG); //-Zi
        /*
        for (const std::wstring& Define : Defines)
        {
            Arguments.emplace_back(L"-D");
            Arguments.emplace_back(Define.c_str());
        }*/

        ComPtr<IDxcCompiler3> Compiler;
        ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler)));

        DxcBuffer SourceBuffer;
        SourceBuffer.Ptr = SourceBlob->GetBufferPointer();
        SourceBuffer.Size = SourceBlob->GetBufferSize();
        int IsEncodingKnown;
        SourceBlob->GetEncoding(&IsEncodingKnown, &SourceBuffer.Encoding);

        if (IsEncodingKnown == 0)
        {
            //TODO: Add logging
            return false;
        }

        ComPtr<IDxcResult> CompileResult;
        ThrowIfFailed(Compiler->Compile(&SourceBuffer, Arguments.data(), (uint32)Arguments.size(), nullptr, IID_PPV_ARGS(CompileResult.GetAddressOf())));
        HRESULT HR;
        CompileResult->GetStatus(&HR);

        // Error Handling. Note that this will also include warnings unless disabled.
        ComPtr<IDxcBlobUtf8> Errors;
        CompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(Errors.GetAddressOf()), nullptr);

        if (Errors && Errors->GetStringLength() > 0)
        {
            //TODO: Add logging
            printf("Compile returned code (0x%x), errors/warnings:\n\n %s\n", HR, Errors->GetStringPointer());
        }

        if (FAILED(HR))
        {
            return false;
        }

        ComPtr<IDxcBlob> ShaderBlob; 
        HR = CompileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&ShaderBlob), nullptr);
        
        InShader->m_Path = ShaderPath.string();
        InShader->m_Data = malloc(ShaderBlob->GetBufferSize());
        InShader->m_Size = ShaderBlob->GetBufferSize();

        memcpy(InShader->m_Data, ShaderBlob->GetBufferPointer(), InShader->m_Size);
        InShader->m_Compiled = true;

        ComPtr<IDxcBlob> ReflectionData;
        CompileResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(ReflectionData.GetAddressOf()), nullptr);
        DxcBuffer ReflectionBuffer;
        ReflectionBuffer.Ptr = ReflectionData->GetBufferPointer();
        ReflectionBuffer.Size = ReflectionData->GetBufferSize();
        ReflectionBuffer.Encoding = 0;
        Utils->CreateReflection(&ReflectionBuffer, IID_PPV_ARGS(InShader->m_Reflection.GetAddressOf()));
        
        ProcessReflection(InShader);

        return true;
    }

    bool ShaderCompiler::ProcessReflection(Shader* InShader)
    {
        static const std::string_view RootConstantsName { "$Globals" };

        static const D3D12_SHADER_VISIBILITY ShaderVisibility[EShaderType::NumOfElements] = {
            D3D12_SHADER_VISIBILITY_VERTEX,
            D3D12_SHADER_VISIBILITY_HULL,
            D3D12_SHADER_VISIBILITY_DOMAIN,
            D3D12_SHADER_VISIBILITY_GEOMETRY,
            D3D12_SHADER_VISIBILITY_PIXEL,
            D3D12_SHADER_VISIBILITY_AMPLIFICATION,
            D3D12_SHADER_VISIBILITY_MESH,
            D3D12_SHADER_VISIBILITY_ALL
        };

        D3D12_SHADER_DESC ShaderDescription {};
        InShader->m_Reflection->GetDesc(&ShaderDescription);

        UINT64 RequiredFlags = InShader->m_Reflection->GetRequiresFlags();
        InShader->m_ResourceDescriptorsHeapIndexing = (RequiredFlags & D3D_SHADER_REQUIRES_RESOURCE_DESCRIPTOR_HEAP_INDEXING) != 0;
        InShader->m_SamplerDescriptorsHeapIndexing = (RequiredFlags & D3D_SHADER_REQUIRES_SAMPLER_DESCRIPTOR_HEAP_INDEXING) != 0;
        
        InShader->m_BoundedResources.clear();
        for (uint32 ResourceID = 0; ResourceID < ShaderDescription.BoundResources; ResourceID++)
        {
            D3D12_SHADER_INPUT_BIND_DESC BindDescription{};
            InShader->m_Reflection->GetResourceBindingDesc(ResourceID, &BindDescription);

            bool bAlreadyAdded = false;
            for (const Shader::BoundedResource& Res : InShader->m_BoundedResources)
            {
                if (Res.m_Name == BindDescription.Name)
                {
                    bAlreadyAdded = true;
                    break;
                }
            }

            if (bAlreadyAdded)
            {
                continue;
            }
            
            if (BindDescription.Name == RootConstantsName)
            {
                ID3D12ShaderReflectionConstantBuffer* ReflectionConstantBuffer;
                //for (uint32 ID = 0; ID < ShaderDescription.ConstantBuffers; ID++)
                //{
                    ReflectionConstantBuffer = InShader->m_Reflection->GetConstantBufferByIndex(0);

                    D3D12_SHADER_BUFFER_DESC ShaderBufferDesc {};
                    ReflectionConstantBuffer->GetDesc(&ShaderBufferDesc);

                    uint32 WastedMemory = 0;
                    uint32 CurrentOffset = 0;
                    //ID3D12ShaderReflectionVariable* ReflectionVariable;
                    for (uint32 ID = 0; ID < ShaderBufferDesc.Variables; ID++)
                    {
                        //ReflectionVariable = ReflectionConstantBuffer->GetVariableByIndex(ID);

                        D3D12_SHADER_VARIABLE_DESC ShaderVariableDesc {};
                        ReflectionConstantBuffer->GetVariableByIndex(ID)->GetDesc(&ShaderVariableDesc);
                        
                        if (ShaderVariableDesc.StartOffset > CurrentOffset)
                        {
                            WastedMemory += ShaderVariableDesc.StartOffset - CurrentOffset;
                            CurrentOffset = ShaderVariableDesc.StartOffset;
                        }
                        
                        InShader->m_BoundedResources.emplace_back( Shader::BoundedResource { ShaderVariableDesc.Name, (uint8)ID, (uint8)(ShaderVariableDesc.Size / 4), Shader::EResourceType::DirectConstant, ShaderVisibility[InShader->m_ShaderType] });
                    }
                //}

                continue;
            }

            switch (BindDescription.Type)
            {
                case D3D_SIT_CBUFFER:
                    InShader->m_BoundedResources.emplace_back( Shader::BoundedResource { BindDescription.Name, (uint8)BindDescription.BindPoint, (uint8)BindDescription.Space, Shader::EResourceType::ConstantBufferView, ShaderVisibility[InShader->m_ShaderType] });
                    break;
                case D3D_SIT_TBUFFER:
                case D3D_SIT_TEXTURE:
                    InShader->m_BoundedResources.emplace_back( Shader::BoundedResource { BindDescription.Name, (uint8)BindDescription.BindPoint, (uint8)BindDescription.Space, Shader::EResourceType::ShaderResourceView, ShaderVisibility[InShader->m_ShaderType] });
                    break;
                case D3D_SIT_UAV_RWTYPED:
                    InShader->m_BoundedResources.emplace_back( Shader::BoundedResource { BindDescription.Name, (uint8)BindDescription.BindPoint, (uint8)BindDescription.Space, Shader::EResourceType::UnorderedAccessView, ShaderVisibility[InShader->m_ShaderType] });
                    break;
                case D3D_SIT_STRUCTURED:
                case D3D_SIT_BYTEADDRESS:
                    InShader->m_BoundedResources.emplace_back( Shader::BoundedResource { BindDescription.Name, (uint8)BindDescription.BindPoint, (uint8)BindDescription.Space, Shader::EResourceType::ShaderResourceViewUntyped, ShaderVisibility[InShader->m_ShaderType] });
                    break;
                case D3D_SIT_UAV_RWSTRUCTURED:
                case D3D_SIT_UAV_RWBYTEADDRESS:
                    InShader->m_BoundedResources.emplace_back( Shader::BoundedResource { BindDescription.Name, (uint8)BindDescription.BindPoint, (uint8)BindDescription.Space, Shader::EResourceType::UnorderedAccessViewUntyped, ShaderVisibility[InShader->m_ShaderType] });
                    break;
                case D3D_SIT_SAMPLER:
                    InShader->m_BoundedResources.emplace_back( Shader::BoundedResource { BindDescription.Name, (uint8)BindDescription.BindPoint, (uint8)BindDescription.Space, Shader::EResourceType::Sampler, ShaderVisibility[InShader->m_ShaderType] });
                    break;
                default:
                    break;
            }
        }
        
        return false;
    }

    bool ShaderCompiler::CompileRootSignature(ERenderingPipelineType PipelineType, std::vector<Shader*> InShaders, RootSignature* InRootSignature)
    {
        if (InRootSignature->IsValid())
        {
            InRootSignature->Release();
        }

        // Do not process anything unless all required shaders are compiled
        for (uint32 ShaderID = 0; ShaderID < InShaders.size(); ShaderID++)
        {
            if (IsShaderRequired(PipelineType, InShaders[ShaderID]->GetShaderType()) && !InShaders[ShaderID]->IsCompiled())
            {
                return false;
            }
        }

        int8 AvailableRootSignatureDWORDs = 64;
        int8 DWORDsRequiredByRootConstants = 0;

        std::map<uint64, Shader::BoundedResource> RootConstants;
        std::map<uint64, Shader::BoundedResource> RootDescriptors;
        std::map<uint64, Shader::BoundedResource> RootDescriptorsTable;
        std::map<uint64, Shader::BoundedResource> Samplers;
        for (uint32 ShaderID = 0; ShaderID < InShaders.size(); ShaderID++)
        {
            for (Shader::BoundedResource& Resource : InShaders[ShaderID]->m_BoundedResources)
            {
                uint64 HashedName = std::hash<std::string_view>{}(Resource.m_Name);
                if (Resource.m_Type == Shader::EResourceType::DirectConstant)
                {
                    auto It = RootConstants.find(HashedName);
                    if (It != RootConstants.end())
                    {
                        It->second.m_Visibility = D3D12_SHADER_VISIBILITY_ALL;
                    }
                    else
                    {
                        RootConstants.emplace(HashedName, Resource);
                        DWORDsRequiredByRootConstants += 2;
                    }
                }
                else if (Resource.m_Type == Shader::EResourceType::ConstantBufferView ||
                        Resource.m_Type == Shader::EResourceType::ShaderResourceViewUntyped ||
                        Resource.m_Type == Shader::EResourceType::UnorderedAccessViewUntyped)
                {
                    auto It = RootDescriptors.find(HashedName);
                    if (It != RootDescriptors.end())
                    {
                        It->second.m_Visibility = D3D12_SHADER_VISIBILITY_ALL;
                    }
                    else
                    {
                        RootDescriptors.emplace(HashedName, Resource);
                    }
                }
                else if (Resource.m_Type == Shader::EResourceType::ShaderResourceView ||
                    Resource.m_Type == Shader::EResourceType::UnorderedAccessView)
                {
                    auto It = RootDescriptorsTable.find(HashedName);
                    if (It != RootDescriptorsTable.end())
                    {
                        It->second.m_Visibility = D3D12_SHADER_VISIBILITY_ALL;
                    }
                    else
                    {
                        RootDescriptorsTable.emplace(HashedName, Resource);
                    }
                }
                else if (Resource.m_Type == Shader::EResourceType::Sampler)
                {
                    auto It = Samplers.find(HashedName);
                    if (It != Samplers.end())
                    {
                        It->second.m_Visibility = D3D12_SHADER_VISIBILITY_ALL;
                    }
                    else
                    {
                        Samplers.emplace(HashedName, Resource);
                    }
                }
                else
                {
                    // TODO: Add logging - 
                }
            }
        }
        
        uint32 ConstantParamNum = 0;
        uint32 SRVParamNum = 0;
        uint32 UAVParamNum = 0;
        uint32 SamplerParamNum = 0;

        auto PrepareOrderedTable = [](std::map<uint64, Shader::BoundedResource>& Map, std::vector<Shader::BoundedResource>& Out) {

			static auto SortByBindPoint = [](const Shader::BoundedResource& Arg1, const Shader::BoundedResource& Arg2) {
				return Arg1.m_BindPoint < Arg2.m_BindPoint;
			};

            Out.reserve(Map.size());
			for (auto& Entry : Map)
			{
				Out.push_back(Entry.second);
			}

            std::sort(Out.begin(), Out.end(), SortByBindPoint);
        };

		std::vector<Shader::BoundedResource> OrderedRootConstants;
		std::vector<Shader::BoundedResource> OrderedRootDescriptors;
		std::vector<Shader::BoundedResource> OrderedRootDescriptorsTable;

        PrepareOrderedTable(RootConstants, OrderedRootConstants);
        PrepareOrderedTable(RootDescriptors, OrderedRootDescriptors);
        PrepareOrderedTable(RootDescriptorsTable, OrderedRootDescriptorsTable);

        std::map<HashValue, uint32> Bindings;
        std::hash<std::string_view> Hasher;
        uint32 BindingID = 0;

        for (const Shader::BoundedResource& Entry : OrderedRootConstants)
        {
			Bindings.emplace(Hasher(Entry.m_Name), BindingID);
			BindingID++;

            InRootSignature->AddConstantParameter(Entry.m_Space, Entry.m_BindPoint, Entry.m_Visibility);
        }
        
        for (const Shader::BoundedResource& Entry : OrderedRootDescriptors)
		{
			Bindings.emplace(Hasher(Entry.m_Name), BindingID);
			BindingID++;

            if (Entry.m_Type == Shader::EResourceType::ConstantBufferView)
            {
                InRootSignature->AddCBVParameter(Entry.m_BindPoint, Entry.m_Visibility, Entry.m_Space);
            }
            else if (Entry.m_Type == Shader::EResourceType::ShaderResourceViewUntyped)
            {
                InRootSignature->AddSRVParameter(Entry.m_BindPoint, Entry.m_Visibility, Entry.m_Space);
            }
            else if (Entry.m_Type == Shader::EResourceType::UnorderedAccessViewUntyped)
            {
                InRootSignature->AddUAVParameter(Entry.m_BindPoint, Entry.m_Visibility, Entry.m_Space);
            }
        }

        bool bCloseDescriptorTable = false;
        auto BeginDescriptorTable = [&bCloseDescriptorTable, &InRootSignature]() {
            if (!bCloseDescriptorTable)
            {
                bCloseDescriptorTable = true;
                InRootSignature->BeginDescriptorTableParameter();
            }
        };

        for (const Shader::BoundedResource& Entry : OrderedRootDescriptorsTable)
		{
			Bindings.emplace(Hasher(Entry.m_Name), BindingID);
			BindingID++;

            if (Entry.m_Type == Shader::EResourceType::ConstantBufferView)
            {
                BeginDescriptorTable();
                InRootSignature->AddCBVRange(1, Entry.m_BindPoint, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, Entry.m_Space);
            }
            else if (Entry.m_Type == Shader::EResourceType::ShaderResourceView)
            {
                BeginDescriptorTable();
                InRootSignature->AddSRVRange(1, Entry.m_BindPoint, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, Entry.m_Space);
            }
            else if (Entry.m_Type == Shader::EResourceType::UnorderedAccessView)
            {
                BeginDescriptorTable();
                InRootSignature->AddUAVRange(1, Entry.m_BindPoint, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, Entry.m_Space);
            }
            else if (Entry.m_Type == Shader::EResourceType::Sampler)
            {
                BeginDescriptorTable();
                InRootSignature->AddSamplerRange(1, Entry.m_BindPoint, D3D12_DESCRIPTOR_RANGE_FLAG_NONE, Entry.m_Space);
            }
        }

        if (bCloseDescriptorTable)
        {
            InRootSignature->EndDescriptorTableParameter();
        }

        for (auto& Entry : Samplers)
        {
            InRootSignature->AddStaticSampler(Entry.second.m_BindPoint, Entry.second.m_Visibility, Entry.second.m_Space);
        }

        if (InRootSignature->EndRootSignature(DXContext::Device(), D3D12_ROOT_SIGNATURE_FLAG_NONE))
        {
            InRootSignature->SetBindingMap(std::move(Bindings));
        }
    }
}
