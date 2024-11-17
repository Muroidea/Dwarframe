#ifndef WORLD_H
#define WORLD_H

namespace Dwarframe {

	class Entity;
	class Renderable;
	class RenderPass;

	class World
	{
	public:
		World();

		void SetRenderPasses(std::vector<RenderPass*>* RenderPasses);
		void AddNewGameObject(Entity* NewEntity);
		void AddRenderable(Renderable* NewRenderable);
		void Update(float Delta);

#if WITH_EDITOR
		void RenderGameObjects();
#endif

	private:
		int32 SelectedEntity = -1;
		std::vector<Entity*> m_Entities {};

		std::vector<Entity*> m_AddedEntities {};
		std::vector<Renderable*> m_AddRenderable {};
		std::vector<RenderPass*>* m_AvailablePasses {};
	};

}

#endif // !WORLD_H