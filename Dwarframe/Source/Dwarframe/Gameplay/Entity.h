#ifndef ENTITY_H
#define ENTITY_H

namespace Dwarframe {

	class Component;
	class Material;
	class Mesh;
	class Renderable;
	class World;

	struct Transform
	{
	public:
		Transform()
		{
			m_Translation = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
			m_Scale = { 1.0f, 1.0f, 1.0f, 0.0f };
			bHasChanged = false;
		}

		inline DirectX::XMVECTOR GetTranslation() const
		{
			return m_Translation;
		}

		inline DirectX::XMVECTOR GetRotation() const
		{
			return m_Rotation;
		}

		inline DirectX::XMVECTOR GetScale() const
		{
			return m_Scale;
		}

		inline void SetTranslation(DirectX::XMVECTOR NewTranslation)
		{
			m_Translation = NewTranslation;
			bHasChanged = true;
		}

		inline void SetRotation(DirectX::XMVECTOR NewRotation)
		{
			m_Translation = NewRotation;
			bHasChanged = true;
		}

		inline void SetScale(DirectX::XMVECTOR NewScale)
		{
			m_Translation = NewScale;
			bHasChanged = true;
		}

		inline void Translate(DirectX::XMVECTOR Translation)
		{
			m_Translation = DirectX::XMVectorAdd(m_Translation, Translation);
			bHasChanged = true;
		}

		inline void Rotate(DirectX::XMVECTOR Rotation)
		{
			m_Rotation = DirectX::XMVectorAdd(m_Rotation, Rotation);
			bHasChanged = true;
		}

		inline void Scale(DirectX::XMVECTOR Scale)
		{
			m_Scale = DirectX::XMVectorAdd(m_Scale, Scale);
			bHasChanged = true;
		}

		inline DirectX::XMMATRIX GetTransform()
		{
			DirectX::XMMATRIX Matrix = DirectX::XMMatrixScalingFromVector(m_Scale);
			Matrix *= DirectX::XMMatrixRotationQuaternion(m_Rotation);
			Matrix *= DirectX::XMMatrixTranslationFromVector(m_Translation);

			return Matrix;
		}

		inline bool HasChanged() { return bHasChanged; }

	private:
		DirectX::XMVECTOR m_Translation;
		DirectX::XMVECTOR m_Rotation;
		DirectX::XMVECTOR m_Scale;

		bool bHasChanged;
	};

	class Entity final : public IResourceInspectorExtender
	{
	public:
		Entity(std::string_view Name, World* InWorld);
		
		inline std::string_view GetName() const { return m_Name; }
		
#ifdef WITH_EDITOR
		virtual void RenderOptions() override;
#endif
		
		inline void MarkRenderDataDirty() { bRenderDataDirty = true; }

		void TryCreateRenderable();

		inline DirectX::XMMATRIX GetTransformMatrix()
		{
			if (m_Transfom.HasChanged())
			{

			}
		}

	private:
		// Transform info
		Transform m_Transfom;
		DirectX::XMMATRIX m_WorldTransform;

		World* m_World;

		std::string_view m_Name;
		std::vector<Component*> m_Components;

		Entity* Parent;
		std::vector<Entity*> m_Children;

		// TODO: Renderable has to be handled differently
		bool bRenderDataDirty = false;
		Renderable* m_RelatedRenderable;

		Mesh* m_Mesh;
		Material* m_Material;

#ifdef WITH_EDITOR
		// Editor data
		std::string MeshButtonName = "Select Mesh Asset";
		std::string MaterialButtonName = "Select Material Asset";
#endif

		friend Renderable;
	};

}

#endif // !ENTITY_H

