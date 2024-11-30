#ifndef WORLD_H
#define WORLD_H

namespace Dwarframe {
	
	class Camera;
	class Entity;
	class Renderable;
	class RenderPass;
	
	enum EEntityOperation
	{
		Add,
		Remove,
		Modify
	};

	class World
	{
	public:
		World();
		~World();

		void SetRenderPasses(std::vector<RenderPass*>* Passes);

		void CreateEntity(std::string Name);
#if WITH_EDITOR
		void UpdateEntity(Entity* EntityToUpdate);
#endif
		void DestroyEntity(Entity* EntityToRemove);

		void AddRenderable(Renderable* RenderableToAdd);
		void RemoveRenderable(Renderable* RenderableToRemove);

		void Update(float DeltaTime);

		const std::vector<Entity*>& GetEntities() const { return m_Entities; }

#if WITH_EDITOR
		const std::vector<Entity*>& GetUnparentedEntities() const { return m_UnparentedEntities; }
		void RenderWorldOutliner();
#endif

	private:
		Camera* m_DefaultCamera;

		int32 m_SelectedEntity = -1;
		std::vector<Entity*> m_Entities {};

#if WITH_EDITOR
		std::vector<Entity*> m_UnparentedEntities {};
#endif

		std::vector<Entity*> m_EntitiesToAdd {};
		std::vector<Entity*> m_EntitiesToRemove {};
		std::vector<Entity*> m_EntitiesToUpdate {};
		// TODO: Maybe we should store all changes in one vector?
		std::vector<std::pair<Entity*, EEntityOperation>> m_EntitiesOperations {};

		std::vector<Renderable*> m_RenderablesToAdd {};
		std::vector<Renderable*> m_RenderablesToRemove {};

		std::vector<RenderPass*>* m_AvailablePasses {};
	};

}

#endif // !WORLD_H