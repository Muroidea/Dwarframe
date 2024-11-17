#include "pch.h"
#include "Entity.h"

#include "ImGUI/imgui.h"
#include "Dwarframe/Gameplay/World.h"
#include "Dwarframe/Renderer/RenderPasses/Renderable.h"
#include "Dwarframe/Resources/Mesh.h"
#include "Dwarframe/Resources/Material.h"

namespace Dwarframe {

	Entity::Entity(std::string_view Name, World* InWorld)
		: m_World(InWorld), m_Name(Name)
	{

	}
	
#ifdef WITH_EDITOR
	void Entity::RenderOptions()
	{
		ImGui::Button(MeshButtonName.c_str());

		if (ImGui::BeginDragDropTarget())
        {
            //if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
            if (const ImGuiPayload* Payload = ImGui::GetDragDropPayload())
            {
				if (Payload->DataSize == sizeof(Mesh*))
				{
					GameAsset* Asset = *static_cast<GameAsset**>(Payload->Data);

					if (Asset && TypeID::Value<Mesh>() == Asset->GetID())
					{
						m_Mesh = static_cast<Mesh*>(Asset);
						MeshButtonName = m_Mesh->GetAssetName();

						if (!m_RelatedRenderable)
						{
							TryCreateRenderable();
						}

						MarkRenderDataDirty();
					}
				}
            }

            ImGui::EndDragDropTarget();
        }

		ImGui::Button(MaterialButtonName.c_str());

		if (ImGui::BeginDragDropTarget())
        {
            //if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
            if (const ImGuiPayload* Payload = ImGui::GetDragDropPayload())
            {
				if (Payload->DataSize == sizeof(GameAsset*))
				{
					GameAsset* Asset = *static_cast<GameAsset**>(Payload->Data);

					if (Asset && TypeID::Value<Material>() == Asset->GetID())
					{
						m_Material = static_cast<Material*>(Asset);
						MaterialButtonName = m_Material->GetAssetName();

						if (!m_RelatedRenderable)
						{
							TryCreateRenderable();
						}

						MarkRenderDataDirty();
					}
				}
            }

            ImGui::EndDragDropTarget();
        }
		
	}

	void Entity::TryCreateRenderable()
	{
		if (!m_Material || !m_Mesh)
		{
			return;
		}

		m_World->AddRenderable(new Renderable(this));
	}

#endif

}