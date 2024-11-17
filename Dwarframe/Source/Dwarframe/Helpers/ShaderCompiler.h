#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H

#include "Dwarframe/Renderer/GPUResources/Shader.h"

namespace Dwarframe {

	class RootSignature;

	class ShaderCompiler
	{
	public:
		static bool Load(Shader* InShader);
		static bool Compile(Shader* InShader);
		static bool ProcessReflection(Shader* InShader);
		static bool CompileRootSignature(ERenderingPipelineType PipelineType, std::vector<Shader*> InShaders, RootSignature* InRootSignature);

	private:
		ShaderCompiler() = default;
		ShaderCompiler(const ShaderCompiler&) = default;
		ShaderCompiler& operator=(const ShaderCompiler&) = default;
	};

}

#endif // !SHADERCOMPILER_H



