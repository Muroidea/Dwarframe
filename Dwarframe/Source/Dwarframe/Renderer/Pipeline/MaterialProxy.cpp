#include "pch.h"
#include "MaterialProxy.h"

#include "Dwarframe/Renderer/GPUResources/Shader.h"

namespace Dwarframe {

	MaterialProxy::~MaterialProxy()
	{
		if (m_AmplifierShader)
		{
			delete m_AmplifierShader;
		}

		if (m_MeshShader)
		{
			delete m_MeshShader;
		}

		if (m_PixelShader)
		{
			delete m_PixelShader;
		}
	}

	void MaterialProxy::LoadAmplifierShader(std::string ShaderName)
	{
		if (m_AmplifierShader)
		{
			delete m_AmplifierShader;
		}

		m_AmplifierShader = new Shader(ShaderName);
	}

	void MaterialProxy::LoadMeshShader(std::string ShaderName)
	{
		if (m_MeshShader)
		{
			delete m_MeshShader;
		}

		m_MeshShader = new Shader(ShaderName);
	}

	void MaterialProxy::LoadPixelShader(std::string ShaderName)
	{
		if (m_PixelShader)
		{
			delete m_PixelShader;
		}

		m_PixelShader = new Shader(ShaderName);
	}

}