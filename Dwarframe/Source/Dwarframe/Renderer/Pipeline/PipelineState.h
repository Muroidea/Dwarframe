#ifndef PIPELINESTATE_H
#define PIPELINESTATE_H

#include "Dwarframe/Resources/Material.h"
#include "Dwarframe/Renderer/Pipeline/MaterialProxy.h"
#include "Dwarframe/Renderer/GPUResources/Shader.h"

namespace Dwarframe {
	
	template <typename InnerType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE SubobjectType>
	struct alignas(void*) StreamingSubobject
	{
	public:
		D3D12_PIPELINE_STATE_SUBOBJECT_TYPE _Type = SubobjectType;
		InnerType _Inner {};
	};

	struct MESH_PIPELINE_STATE_STREAM
	{
		StreamingSubobject<D3D12_PIPELINE_STATE_FLAGS, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS> Flags;
		StreamingSubobject<D3D12_NODE_MASK, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK> NodeMask;
		StreamingSubobject<ID3D12RootSignature*, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE> pRootSignature;
		StreamingSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS> PS;
		StreamingSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS> AS;
		StreamingSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS> MS;
		StreamingSubobject<D3D12_BLEND_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND> BlendState;
		StreamingSubobject<D3D12_DEPTH_STENCIL_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL> DepthStencilState;
		StreamingSubobject<DXGI_FORMAT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT> DSVFormat;
		StreamingSubobject<D3D12_RASTERIZER_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER> RasterizerState;
		StreamingSubobject<D3D12_RT_FORMAT_ARRAY, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS> RTVFormats;
		StreamingSubobject<DXGI_SAMPLE_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC> SampleDesc;
		StreamingSubobject<UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK> SampleMask;
		StreamingSubobject<D3D12_CACHED_PIPELINE_STATE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO> CachedPSO;
		//StreamingSubobject<D3D12_PRIMITIVE_TOPOLOGY_TYPE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY> PrimitiveTopologyType;
		//StreamingSubobject<D3D12_VIEW_INSTANCING_DESC., D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING> ViewInstancingDesc;
	};

	struct CLASSIC_PIPELINE_STATE_STREAM
	{
		StreamingSubobject<D3D12_PIPELINE_STATE_FLAGS, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS> Flags;
		StreamingSubobject<D3D12_NODE_MASK, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK> NodeMask;
		StreamingSubobject<ID3D12RootSignature*, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE> pRootSignature;
		StreamingSubobject<D3D12_INPUT_LAYOUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT> InputLayout;
		StreamingSubobject<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE> IBStripCutValue;
		StreamingSubobject<D3D12_PRIMITIVE_TOPOLOGY_TYPE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY> PrimitiveTopologyType;
		StreamingSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS> VS;
		StreamingSubobject<D3D12_STREAM_OUTPUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT> StreamOutput;
		StreamingSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS> PS;
		StreamingSubobject<D3D12_BLEND_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND> BlendState;
		StreamingSubobject<D3D12_DEPTH_STENCIL_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL> DepthStencilState;
		StreamingSubobject<DXGI_FORMAT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT> DSVFormat;
		StreamingSubobject<D3D12_RASTERIZER_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER> RasterizerState;
		StreamingSubobject<D3D12_RT_FORMAT_ARRAY, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS> RTVFormats;
		StreamingSubobject<DXGI_SAMPLE_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC> SampleDesc;
		StreamingSubobject<UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK> SampleMask;
		StreamingSubobject<D3D12_CACHED_PIPELINE_STATE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO> CachedPSO;
	};

	class PipelineStateBase
	{
	public:
		PipelineStateBase() = default;
		~PipelineStateBase()
		{
			m_PipelineState.Reset();
			m_PipelineState = nullptr;
		}

		inline ID3D12PipelineState* GetPipelineState() const { return m_PipelineState.Get(); }

	protected:
		ComPtr<ID3D12PipelineState> m_PipelineState;
	};

	template <typename StateStream>
	class PipelineState : public PipelineStateBase
	{
	public:
		bool CreatePipelineState(ID3D12Device8* D3D12Device, const Material* InMaterial)
		{
			// TODO: Better logging!
			std::cout << "Please use one of specialized templates!\n";
		}

		bool CreatePipelineState(ID3D12Device8* D3D12Device, const MaterialProxy* InMaterial)
		{
			// TODO: Better logging!
			std::cout << "Please use one of specialized templates!\n";
		}
	};

	template <>
	class PipelineState<MESH_PIPELINE_STATE_STREAM> : public PipelineStateBase
	{
	public:
		bool CreatePipelineState(ID3D12Device8* D3D12Device, const Material* InMaterial)
		{
			//****** Mesh Pipeline state ******
			MeshStateStream.pRootSignature._Inner = InMaterial->GetRootSignature();

			MeshStateStream.AS._Inner.BytecodeLength = 0;
			MeshStateStream.AS._Inner.pShaderBytecode = nullptr;

			MeshStateStream.MS._Inner.BytecodeLength = InMaterial->GetShaderSize(EShaderType::MeshShader);
			MeshStateStream.MS._Inner.pShaderBytecode = InMaterial->GetShaderBinary(EShaderType::MeshShader);

			MeshStateStream.PS._Inner.BytecodeLength = InMaterial->GetShaderSize(EShaderType::PixelShader);
			MeshStateStream.PS._Inner.pShaderBytecode = InMaterial->GetShaderBinary(EShaderType::PixelShader);

			MeshStateStream.RasterizerState._Inner.FillMode = D3D12_FILL_MODE_SOLID;
			MeshStateStream.RasterizerState._Inner.CullMode = D3D12_CULL_MODE_BACK;
			MeshStateStream.RasterizerState._Inner.FrontCounterClockwise = false;
			MeshStateStream.RasterizerState._Inner.DepthBias = 0;
			MeshStateStream.RasterizerState._Inner.DepthBiasClamp = 0.0f;
			MeshStateStream.RasterizerState._Inner.SlopeScaledDepthBias = 0.0f;
			MeshStateStream.RasterizerState._Inner.DepthClipEnable = false;
			MeshStateStream.RasterizerState._Inner.MultisampleEnable = false;
			MeshStateStream.RasterizerState._Inner.AntialiasedLineEnable = false;
			MeshStateStream.RasterizerState._Inner.ForcedSampleCount = 0;

			MeshStateStream.RTVFormats._Inner.NumRenderTargets = 1;
			MeshStateStream.RTVFormats._Inner.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			MeshStateStream.DSVFormat._Inner = DXGI_FORMAT_D32_FLOAT;

			MeshStateStream.BlendState._Inner.AlphaToCoverageEnable = false;
			MeshStateStream.BlendState._Inner.IndependentBlendEnable = false;
			MeshStateStream.BlendState._Inner.RenderTarget[0].BlendEnable = false;
			MeshStateStream.BlendState._Inner.RenderTarget[0].LogicOpEnable = false;
			// Blending of color component
			MeshStateStream.BlendState._Inner.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			MeshStateStream.BlendState._Inner.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
			MeshStateStream.BlendState._Inner.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			// Blending of alpha component
			MeshStateStream.BlendState._Inner.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
			MeshStateStream.BlendState._Inner.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			MeshStateStream.BlendState._Inner.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

			MeshStateStream.BlendState._Inner.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
			MeshStateStream.BlendState._Inner.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			MeshStateStream.DepthStencilState._Inner.DepthEnable = true;
			MeshStateStream.DepthStencilState._Inner.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			MeshStateStream.DepthStencilState._Inner.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

			MeshStateStream.DepthStencilState._Inner.StencilEnable = false;
			MeshStateStream.DepthStencilState._Inner.StencilReadMask = 0;
			MeshStateStream.DepthStencilState._Inner.StencilWriteMask = 0;

			MeshStateStream.DepthStencilState._Inner.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

			MeshStateStream.DepthStencilState._Inner.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

			MeshStateStream.SampleMask._Inner = 0xFFFFFFFF;
			MeshStateStream.SampleDesc._Inner.Count = 1;
			MeshStateStream.SampleDesc._Inner.Quality = 0;

			MeshStateStream.NodeMask._Inner.NodeMask = 0;
			MeshStateStream.CachedPSO._Inner.CachedBlobSizeInBytes = 0;
			MeshStateStream.CachedPSO._Inner.pCachedBlob = nullptr;
			MeshStateStream.Flags._Inner = D3D12_PIPELINE_STATE_FLAG_NONE; // alternatively D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG

			D3D12_PIPELINE_STATE_STREAM_DESC PipelineStateStreamDesc{};
			PipelineStateStreamDesc.SizeInBytes = sizeof(MESH_PIPELINE_STATE_STREAM);
			PipelineStateStreamDesc.pPipelineStateSubobjectStream = &MeshStateStream;

			ThrowIfFailed(D3D12Device->CreatePipelineState(&PipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

			return true;
		}

		bool CreatePipelineState(ID3D12Device8* D3D12Device, const MaterialProxy* InMaterial)
		{
			//****** Mesh Pipeline state ******
			MeshStateStream.pRootSignature._Inner = InMaterial->m_RootSignature.GetRootSignature();

			MeshStateStream.AS._Inner.BytecodeLength = 0;
			MeshStateStream.AS._Inner.pShaderBytecode = nullptr;

			MeshStateStream.MS._Inner.BytecodeLength = InMaterial->m_MeshShader->GetShaderSize();
			MeshStateStream.MS._Inner.pShaderBytecode = InMaterial->m_MeshShader->GetShaderBinary();

			MeshStateStream.PS._Inner.BytecodeLength = InMaterial->m_PixelShader->GetShaderSize();
			MeshStateStream.PS._Inner.pShaderBytecode = InMaterial->m_PixelShader->GetShaderBinary();

			MeshStateStream.RasterizerState._Inner.FillMode = D3D12_FILL_MODE_SOLID;
			MeshStateStream.RasterizerState._Inner.CullMode = D3D12_CULL_MODE_BACK;
			MeshStateStream.RasterizerState._Inner.FrontCounterClockwise = false;
			MeshStateStream.RasterizerState._Inner.DepthBias = 0;
			MeshStateStream.RasterizerState._Inner.DepthBiasClamp = 0.0f;
			MeshStateStream.RasterizerState._Inner.SlopeScaledDepthBias = 0.0f;
			MeshStateStream.RasterizerState._Inner.DepthClipEnable = false;
			MeshStateStream.RasterizerState._Inner.MultisampleEnable = false;
			MeshStateStream.RasterizerState._Inner.AntialiasedLineEnable = false;
			MeshStateStream.RasterizerState._Inner.ForcedSampleCount = 0;

			MeshStateStream.RTVFormats._Inner.NumRenderTargets = 1;
			MeshStateStream.RTVFormats._Inner.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			MeshStateStream.DSVFormat._Inner = DXGI_FORMAT_D32_FLOAT;

			MeshStateStream.BlendState._Inner.AlphaToCoverageEnable = false;
			MeshStateStream.BlendState._Inner.IndependentBlendEnable = false;
			MeshStateStream.BlendState._Inner.RenderTarget[0].BlendEnable = false;
			MeshStateStream.BlendState._Inner.RenderTarget[0].LogicOpEnable = false;
			// Blending of color component
			MeshStateStream.BlendState._Inner.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			MeshStateStream.BlendState._Inner.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
			MeshStateStream.BlendState._Inner.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			// Blending of alpha component
			MeshStateStream.BlendState._Inner.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
			MeshStateStream.BlendState._Inner.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			MeshStateStream.BlendState._Inner.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

			MeshStateStream.BlendState._Inner.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
			MeshStateStream.BlendState._Inner.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			MeshStateStream.DepthStencilState._Inner.DepthEnable = true;
			MeshStateStream.DepthStencilState._Inner.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			MeshStateStream.DepthStencilState._Inner.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

			MeshStateStream.DepthStencilState._Inner.StencilEnable = false;
			MeshStateStream.DepthStencilState._Inner.StencilReadMask = 0;
			MeshStateStream.DepthStencilState._Inner.StencilWriteMask = 0;

			MeshStateStream.DepthStencilState._Inner.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

			MeshStateStream.DepthStencilState._Inner.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			MeshStateStream.DepthStencilState._Inner.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

			MeshStateStream.SampleMask._Inner = 0xFFFFFFFF;
			MeshStateStream.SampleDesc._Inner.Count = 1;
			MeshStateStream.SampleDesc._Inner.Quality = 0;

			MeshStateStream.NodeMask._Inner.NodeMask = 0;
			MeshStateStream.CachedPSO._Inner.CachedBlobSizeInBytes = 0;
			MeshStateStream.CachedPSO._Inner.pCachedBlob = nullptr;
			MeshStateStream.Flags._Inner = D3D12_PIPELINE_STATE_FLAG_NONE; // alternatively D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG

			D3D12_PIPELINE_STATE_STREAM_DESC PipelineStateStreamDesc{};
			PipelineStateStreamDesc.SizeInBytes = sizeof(MESH_PIPELINE_STATE_STREAM);
			PipelineStateStreamDesc.pPipelineStateSubobjectStream = &MeshStateStream;

			ThrowIfFailed(D3D12Device->CreatePipelineState(&PipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

			return true;
		}

	private:
		MESH_PIPELINE_STATE_STREAM MeshStateStream;
	};

	using MeshPipelineState = PipelineState<MESH_PIPELINE_STATE_STREAM>;
	using SimplePipelineState = PipelineState<CLASSIC_PIPELINE_STATE_STREAM>;
}

/*
    //****** Classic Pipeline state ******

    D3D12_GRAPHICS_PIPELINE_STATE_DESC ClassicPipelineStateDesc {};
    ClassicPipelineStateDesc.pRootSignature = m_ClassicRootSignature.GetRootSignature();
    ClassicPipelineStateDesc.InputLayout.NumElements = m_VertexLayout.GetNumOfAttributes();
    ClassicPipelineStateDesc.InputLayout.pInputElementDescs = m_VertexLayout.GetInputElementDesc();
    ClassicPipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

    ClassicPipelineStateDesc.VS.BytecodeLength = VertexShader->GetShaderSize();
    ClassicPipelineStateDesc.VS.pShaderBytecode = VertexShader->GetShaderBinary();
    ClassicPipelineStateDesc.DS.BytecodeLength = 0;
    ClassicPipelineStateDesc.DS.pShaderBytecode = nullptr;
    ClassicPipelineStateDesc.HS.BytecodeLength = 0;
    ClassicPipelineStateDesc.HS.pShaderBytecode = nullptr;
    ClassicPipelineStateDesc.GS.BytecodeLength = 0;
    ClassicPipelineStateDesc.GS.pShaderBytecode = nullptr;
    ClassicPipelineStateDesc.PS.BytecodeLength = PixelShader->GetShaderSize();
    ClassicPipelineStateDesc.PS.pShaderBytecode = PixelShader->GetShaderBinary();

    ClassicPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    ClassicPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    ClassicPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    ClassicPipelineStateDesc.RasterizerState.FrontCounterClockwise = false;
    ClassicPipelineStateDesc.RasterizerState.DepthBias = 0;
    ClassicPipelineStateDesc.RasterizerState.DepthBiasClamp = 0.0f;
    ClassicPipelineStateDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
    ClassicPipelineStateDesc.RasterizerState.DepthClipEnable = false; 
    ClassicPipelineStateDesc.RasterizerState.MultisampleEnable = false;
    ClassicPipelineStateDesc.RasterizerState.AntialiasedLineEnable = false;
    ClassicPipelineStateDesc.RasterizerState.ForcedSampleCount = 0;
    //PipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    ClassicPipelineStateDesc.StreamOutput.NumEntries = 0;
    ClassicPipelineStateDesc.StreamOutput.NumStrides = 0;
    ClassicPipelineStateDesc.StreamOutput.pBufferStrides = nullptr;
    ClassicPipelineStateDesc.StreamOutput.pSODeclaration = nullptr;
    ClassicPipelineStateDesc.StreamOutput.RasterizedStream = 0;
        
    ClassicPipelineStateDesc.NumRenderTargets = 1;
    ClassicPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    ClassicPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    ClassicPipelineStateDesc.BlendState.AlphaToCoverageEnable = false;
    ClassicPipelineStateDesc.BlendState.IndependentBlendEnable = false;
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = false;
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
    // Blending of color component
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    // Blending of alpha component
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

    ClassicPipelineStateDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    ClassicPipelineStateDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    ClassicPipelineStateDesc.DepthStencilState.DepthEnable = true;
    ClassicPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; 
    ClassicPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    ClassicPipelineStateDesc.DepthStencilState.StencilEnable = false;
    ClassicPipelineStateDesc.DepthStencilState.StencilReadMask = 0;
    ClassicPipelineStateDesc.DepthStencilState.StencilWriteMask = 0;

    ClassicPipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    ClassicPipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    ClassicPipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    ClassicPipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    ClassicPipelineStateDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    ClassicPipelineStateDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    ClassicPipelineStateDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    ClassicPipelineStateDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    ClassicPipelineStateDesc.SampleMask = 0xFFFFFFFF;
    ClassicPipelineStateDesc.SampleDesc.Count = 1;
    ClassicPipelineStateDesc.SampleDesc.Quality = 0;

    ClassicPipelineStateDesc.NodeMask = 0;
    ClassicPipelineStateDesc.CachedPSO.CachedBlobSizeInBytes = 0;
    ClassicPipelineStateDesc.CachedPSO.pCachedBlob = nullptr;
    ClassicPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE; // alternatively D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG

    ThrowIfFailed(m_D3D12Device->CreateGraphicsPipelineState(&ClassicPipelineStateDesc, IID_PPV_ARGS(&m_ClassicPipelineState)));
*/

#endif // !DESCRIPTORTABLE_H