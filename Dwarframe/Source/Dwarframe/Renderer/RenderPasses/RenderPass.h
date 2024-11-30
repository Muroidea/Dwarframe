#ifndef RENDERPASS_H
#define RENDERPASS_H

namespace Dwarframe {

	class Renderable;

	class RenderPass
	{
	public:
		RenderPass(std::string_view Name = "RenderPass");
		
		virtual void AddRenderable(Renderable* InRenderable);
		virtual void RemoveRenderable(Renderable* InRenderable);

		// TODO: Probably it would be nice to have also such variant:
		//virtual void AddRenderables(std::vector<Renderable*> InRenderables);
		//virtual void RemoveRenderables(std::vector<Renderable*> InRenderables);

		virtual void Render() = 0;
		virtual void Update() = 0;

		// TODO: Think if it should be part of RenderPass
		inline void SetViewMatrix(XMMATRIX ViewMatrix) { m_ViewMatrix = ViewMatrix; }
		inline void SetProjectionMatrix(XMMATRIX ProjectionMatrix) { m_ProjectionMatrix = ProjectionMatrix; }

		inline std::string_view GetName() const { return m_Name; }

	protected:
		const std::string_view m_Name;
		std::vector<Renderable*> m_Renderables;
		
		// TODO: Think if it should be part of RenderPass
		XMMATRIX m_ViewMatrix;
		XMMATRIX m_ProjectionMatrix;
	};

}

#endif // !RENDERPASS_H
