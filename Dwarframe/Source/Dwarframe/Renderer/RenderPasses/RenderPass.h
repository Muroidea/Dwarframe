#ifndef RENDERPASS_H
#define RENDERPASS_H

namespace Dwarframe {

	class Renderable;

	class RenderPass
	{
	public:
		RenderPass(std::string Name = "RenderPass");
		
		virtual void AddRenderable(Renderable* InRenderable);

		virtual void Render() = 0;
		virtual void Update() = 0;

	protected:
		const std::string m_Name;
		std::vector<Renderable*> m_Renderables;
	};

}

#endif // !RENDERPASS_H
