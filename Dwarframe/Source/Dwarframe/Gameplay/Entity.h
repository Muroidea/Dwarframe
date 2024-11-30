#ifndef ENTITY_H
#define ENTITY_H

namespace Dwarframe {

	class Component;
	class Material;
	class Mesh;
	class Renderable;
	class World;

	class Entity final : public IResourceInspectorExtender
	{
	public:
		bool DestroyEntity();

		bool SetParent(Entity* NewParent);
		const Entity* GetParent() const { return m_Parent; }

		bool AddChild(Entity* NewChild);
		bool RemoveChild(Entity* ChildToRemove);
		uint32 GetNumOfChildren(bool InAllHierarchy) const;
		const std::vector<Entity*>& GetChildren() const { return m_Children; }

		inline std::string_view GetName() const { return m_Name; }
		inline bool IsMarkedToDestroy() const { return bMarkedToDestroy; }
		
#if WITH_EDITOR
		virtual void RenderProperties() override;
#endif
		
		//inline void MarkRenderDataDirty() { bRenderDataDirty = true; }
		inline void MarkTransformDirty() 
		{ 
			bTransformDirty = true;
			
			for (Entity* const & Ent : m_Children)
			{
				Ent->MarkTransformDirty();
			}
		}

		bool CreateRenderable();
		
	public:
		
		inline bool HasParent() const
		{
			return m_Parent != nullptr;
		}

		inline bool HasChildren() const
		{
			return !m_Children.empty();
		}
		
		inline bool IsTransformDirty() const
		{
			return bTransformDirty;
		}

		inline bool HasRenderable() const
		{
			return m_Renderable != nullptr;
		}

		inline Renderable* GetRenderable() const
		{
			return m_Renderable;
		}

		inline XMVECTOR GetTranslation() const
		{
			return m_Transfom.GetTranslation();
		}

		inline XMVECTOR GetRotation() const
		{
			return m_Transfom.GetRotation();
		}

		inline XMVECTOR GetRotationEuler() const
		{
			return m_Transfom.GetRotationEuler();
		}

		inline XMVECTOR GetScale() const
		{
			return m_Transfom.GetScale();
		}

		inline void SetTranslation(XMVECTOR NewTranslation)
		{
			m_Transfom.SetTranslation(NewTranslation);
			//bTransformDirty = true;
			MarkTransformDirty();
		}

		inline void SetRotation(XMVECTOR NewRotation)
		{
			m_Transfom.SetRotation(NewRotation);
			//bTransformDirty = true;
			MarkTransformDirty();
		}

		inline void SetRotationEuler(XMVECTOR NewRotation)
		{
			m_Transfom.SetRotationEuler(NewRotation);
			//bTransformDirty = true;
			MarkTransformDirty();
		}

		inline void SetScale(XMVECTOR NewScale)
		{
			m_Transfom.SetScale(NewScale);
			//bTransformDirty = true;
			MarkTransformDirty();
		}

		inline void Translate(XMVECTOR Translation)
		{
			m_Transfom.Translate(Translation);
			//bTransformDirty = true;
			MarkTransformDirty();
		}

		inline void Rotate(XMVECTOR Rotation)
		{
			m_Transfom.Rotate(Rotation);
			//bTransformDirty = true;
			MarkTransformDirty();
		}

		inline void RotateEuler(XMVECTOR Rotation)
		{
			m_Transfom.RotateEuler(Rotation);
			//bTransformDirty = true;
			MarkTransformDirty();
		}

		inline void Scale(XMVECTOR Scale)
		{
			m_Transfom.Scale(Scale);
			//bTransformDirty = true;
			MarkTransformDirty();
		}

		inline XMMATRIX GetTransformMatrix()
		{
			if (bTransformDirty)
			{
				if (m_Parent)
				{
					m_WorldTransform = XMMatrixMultiply(m_Transfom.GetTransform(), m_Parent->GetTransformMatrix());
				}
				else
				{
					m_WorldTransform = m_Transfom.GetTransform();
				}

				bTransformDirty = false;
			}

			return m_WorldTransform;
		}

	private:
		// Transform info
		Transform m_Transfom;
		XMMATRIX m_WorldTransform;

		// Inform as if Transform requires update
		// TODO: Render pass might require special way to handle this
		// Even if Tranform is not dirty anymore we need to know if this had changed that frame
		bool bTransformDirty = true;
		//bool bTransformDirtyThisFrame = true;
		bool bMarkedToDestroy = false;

		World* m_World;

		std::string m_Name;
		std::vector<Component*> m_Components;
		
		Entity* m_Parent;
		std::vector<Entity*> m_Children;
		
		// TODO: Renderable has to be handled differently
		//bool bRenderDataDirty = false;
		Renderable* m_Renderable;

		Mesh* m_Mesh;
		Material* m_Material;

#ifdef WITH_EDITOR
		// Editor data
		std::string m_MeshButtonName = "Select Mesh Asset";
		std::string m_MaterialButtonName = "Select Material Asset";
#endif

	private:
		Entity(std::string Name, World* InWorld);
		~Entity();

		void DestroyEntityInternal();

		// TODO: Remove Renderable class at all. All functionality should have Entity itself.
		friend Renderable;
		friend World;
	};
}

#endif // !ENTITY_H