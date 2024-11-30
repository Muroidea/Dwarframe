#include "pch.h"

#include "ImGUI/imgui.h"
#include "Dwarframe/Editor/WidgetsHelper.h"
#include "Dwarframe/Editor/EditorExtender.h"
#include "Dwarframe/Gameplay/World.h"
#include "Dwarframe/Renderer/RenderPasses/Renderable.h"
#include "Dwarframe/Resources/Mesh.h"
#include "Dwarframe/Resources/Material.h"
#include "Entity.h"

namespace Dwarframe {

	Entity::Entity(std::string Name, World* InWorld)
		: m_World(InWorld), m_Name(Name)
	{

	}
	
	Entity::~Entity()
	{
		assert(bMarkedToDestroy);
	}

	void Entity::DestroyEntityInternal()
	{
		m_Children.clear();

		if (m_Parent)
		{
			m_Parent->RemoveChild(this);
		}
		m_Parent = nullptr;

		if (m_Renderable)
		{
			delete m_Renderable;
			m_Renderable = nullptr;
		}

		m_Mesh = nullptr;
		m_Material = nullptr;
	}

	bool Entity::DestroyEntity()
	{
		if (bMarkedToDestroy)
		{
			return false;
		}

		bMarkedToDestroy = true;

		for (Entity* Child : m_Children)
		{
			Child->DestroyEntity();
		}

		m_World->DestroyEntity(this);

		return true;
	}

	bool Entity::SetParent(Entity* NewParent)
	{
		if (m_Parent)
		{
			m_Parent->RemoveChild(this);
		}

		m_Parent = NewParent;
#if WITH_EDITOR
		m_World->UpdateEntity(this);
#endif

		return true;
	}

	bool Entity::AddChild(Entity* NewChild)
	{
		if (m_Parent != NewChild && std::find(m_Children.begin(), m_Children.end(), NewChild) == m_Children.end())
		{
			m_Children.push_back(NewChild);
			NewChild->SetParent(this);

			return true;
		}

		return false;
	}

	bool Entity::RemoveChild(Entity* ChildToRemove)
	{
		std::vector<Entity*>::iterator Res = std::find(m_Children.begin(), m_Children.end(), ChildToRemove);
		if (Res != m_Children.end())
		{
			m_Children.erase(Res);
			return true;
		}

		return false;
	}

	uint32 Entity::GetNumOfChildren(bool InAllHierarchy) const
	{
		uint32 NumOfChildren = m_Children.size();

		if (InAllHierarchy)
		{
			for (const Entity* const& Ent : m_Children)
			{
				NumOfChildren += Ent->GetNumOfChildren(InAllHierarchy);
			}
		}

		return NumOfChildren;
	}

#if WITH_EDITOR
	void Entity::RenderProperties()
	{
		ImGui::Button(m_MeshButtonName.data());

		EditorHelper::AddAssetDragAndDropTarget<Mesh>(m_Mesh, [this](){
			m_MeshButtonName = m_Mesh->GetAssetName();
			CreateRenderable();
		});

		ImGui::Button(m_MaterialButtonName.data());

		EditorHelper::AddAssetDragAndDropTarget<Material>(m_Material, [this]() {
			m_MaterialButtonName = m_Material->GetAssetName();
			CreateRenderable();
		});

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		EditorHelper::DrawTransform(*this);
	}
#endif

	bool Entity::CreateRenderable()
	{
		if (!m_Material || !m_Mesh)
		{
			return false;
		}

		if (m_Renderable == nullptr)
		{
			m_Renderable = new Renderable(this);
			m_World->AddRenderable(m_Renderable);

			return true;
		}
		else
		{
			m_World->RemoveRenderable(m_Renderable);
			m_World->AddRenderable(m_Renderable);
			return true;
		}
	}
}