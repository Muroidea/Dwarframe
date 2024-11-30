#include "pch.h"
#include "RenderPass.h"

namespace Dwarframe {

	RenderPass::RenderPass(std::string_view Name)
		: m_Name(Name)
	{

	}

	void RenderPass::AddRenderable(Renderable* InRenderable)
	{
		m_Renderables.push_back(InRenderable);
	}

	void RenderPass::RemoveRenderable(Renderable* InRenderable)
	{
		m_Renderables.erase(std::find(m_Renderables.begin(), m_Renderables.end(), InRenderable));
	}

}