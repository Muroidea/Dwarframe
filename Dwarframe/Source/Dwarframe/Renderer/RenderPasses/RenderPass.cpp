#include "pch.h"
#include "RenderPass.h"

namespace Dwarframe {

	RenderPass::RenderPass(std::string Name)
		: m_Name(Name)
	{

	}

	void RenderPass::AddRenderable(Renderable* InRenderable)
	{
		m_Renderables.push_back(InRenderable);
	}

}