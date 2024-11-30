#include "pch.h"
#include "World.h"

#include "Dwarframe/Editor/WidgetsHelper.h"
#include "Dwarframe/Core/Input.h"
#include "Dwarframe/Gameplay/Camera.h"
#include "Dwarframe/Gameplay/Entity.h"
#include "Dwarframe/Renderer/RenderPasses/RenderPass.h"

#include "ImGUI/imgui.h"

namespace Dwarframe {

    World::World()
    {
		m_DefaultCamera = new Camera();
    }

	World::~World()
	{
		if (m_DefaultCamera)
		{
			delete m_DefaultCamera;
			m_DefaultCamera = nullptr;
		}
	}

	void World::SetRenderPasses(std::vector<RenderPass*>* Passes)
	{
		m_AvailablePasses = Passes;
	}

	void World::CreateEntity(std::string Name)
    {
		Entity* NewEntity = new Entity(Name, this);

		m_EntitiesToAdd.push_back(NewEntity);
    }

#if WITH_EDITOR
	void World::UpdateEntity(Entity* EntityToUpdate)
	{
		if (EntityToUpdate)
		{
			m_EntitiesToUpdate.push_back(EntityToUpdate);
		}
	}
#endif

	void World::DestroyEntity(Entity* EntityToRemove)
	{
		if (EntityToRemove)
		{
			if (EntityToRemove->HasRenderable())
			{
				m_RenderablesToRemove.push_back(EntityToRemove->GetRenderable());
			}

			m_EntitiesToRemove.push_back(EntityToRemove);
			//m_EntitiesOperations.push_back({EntityToRemove, EEntityOperation::Remove});
		}
	}

	void World::AddRenderable(Renderable* RenderableToAdd)
	{
        if (RenderableToAdd)
		{
			m_RenderablesToAdd.push_back(RenderableToAdd);
        }
	}

	void World::RemoveRenderable(Renderable* RenderableToRemove)
	{
		// We don't store renderables in World so only add it remove list to populate it to render passes
		if (RenderableToRemove)
		{
			m_RenderablesToRemove.push_back(RenderableToRemove);
		}
	}

	void World::Update(float DeltaTime)
	{
		m_DefaultCamera->Update(DeltaTime);
		m_AvailablePasses->operator[](0)->SetViewMatrix(m_DefaultCamera->GetViewMatrix());
		m_AvailablePasses->operator[](0)->SetProjectionMatrix(m_DefaultCamera->GetProjectionMatrix());

		for (Renderable* Ren : m_RenderablesToRemove)
		{
			// TODO: Adding to proper passes when will be more of them
			m_AvailablePasses->operator[](0)->RemoveRenderable(Ren);
		}
		m_RenderablesToRemove.clear();

		for (Entity* Ent : m_EntitiesToRemove)
		{
			// TODO: Rethink - some extra logic like EndPlay() before adding to world?
			
			for (int32 i = 0; i < m_Entities.size(); i++)
			{
				if (m_Entities[i] == Ent)
				{
#if WITH_EDITOR
					if (!m_Entities[i]->HasParent())
					{
						auto It = std::find(m_UnparentedEntities.begin(), m_UnparentedEntities.end(), Ent);
						if (It != m_UnparentedEntities.end())
						{
							m_UnparentedEntities.erase(It);
						}
					}
#endif
					m_Entities.erase(m_Entities.begin() + i);
					Ent->DestroyEntityInternal();
					delete Ent;
				}
			}
		}
		for (Entity* Ent : m_EntitiesToRemove)
		{

		}
		m_EntitiesToRemove.clear();
#if WITH_EDITOR
		for (Entity* Ent : m_EntitiesToUpdate)
		{
			if (Ent->HasParent())
			{
				auto It = std::find(m_UnparentedEntities.begin(), m_UnparentedEntities.end(), Ent);
				if (It != m_UnparentedEntities.end())
				{
					m_UnparentedEntities.erase(It);
				}
			}
			else
			{
				m_UnparentedEntities.push_back(Ent);
			}
		}
		m_EntitiesToUpdate.clear();
#endif

        for (Entity* Ent : m_EntitiesToAdd)
        {
            // TODO: Rethink - some extra logic like BeginPlay() before adding to world?
			m_Entities.push_back(Ent);
#if WITH_EDITOR
			m_UnparentedEntities.push_back(Ent);
#endif
        }
        m_EntitiesToAdd.clear();

        for (Renderable* Ren : m_RenderablesToAdd)
		{
            // TODO: Adding to proper passes when will be more of them
			m_AvailablePasses->operator[](0)->AddRenderable(Ren);
        }
        m_RenderablesToAdd.clear();
    }

#if WITH_EDITOR
    void World::RenderWorldOutliner()
	{
        if (ImGui::Button("Add new entity"))
        {
            CreateEntity("NewEntity");
        }

		if (ImGui::TreeNode("Entities"))
        {
            static ImGuiTreeNodeFlags BaseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
            static bool TestDragAndDrop = true;
            
            for (int32 EntityID = 0; EntityID < m_Entities.size(); EntityID++)
            {
                ImGuiTreeNodeFlags NodeFlags = BaseFlags;
                const bool IsSelected = m_SelectedEntity == EntityID;

                if (IsSelected) 
                { 
                    NodeFlags |= ImGuiTreeNodeFlags_Selected; 
                }

                if (!m_Entities[EntityID]->HasChildren())
				{
					NodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                }

				ImGui::SetNextItemAllowOverlap();
                bool NodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)EntityID, NodeFlags, m_Entities[EntityID]->GetName().data(), EntityID);

				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					m_SelectedEntity = EntityID;
					ImGUIEditor::Get().SetSelectedEntity(m_Entities[EntityID]);
				}

				if (TestDragAndDrop && ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("_Entity", &m_Entities[EntityID], sizeof(Entity));
					ImGui::Text("This is a drag and drop source");
					ImGui::EndDragDropSource();
				}

				if (TestDragAndDrop)
				{
					Entity* Temp;
					EditorHelper::AddEntityDragAndDropTarget(Temp, [&TargetEntity = m_Entities[EntityID], &Temp]() {
						TargetEntity->AddChild(Temp);
					});
				}
                /*
				ImVec2 button1_pos = ImGui::GetCursorScreenPos();
				ImVec2 button2_pos = ImVec2(button1_pos.x + 50.0f, button1_pos.y + 50.0f);
				if (enable_allow_overlap)
					ImGui::SetNextItemAllowOverlap();
				ImGui::Button("Button 1", ImVec2(80, 80));
				ImGui::SetCursorScreenPos(button2_pos);
				ImGui::Button("Button 2", ImVec2(80, 80));
                */

                ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Delete").x - 50.0f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

				std::string Name = "Delete##" + std::to_string(EntityID);
				if (ImGui::SmallButton(Name.c_str()))
				{
					m_SelectedEntity = -1;
					ImGUIEditor::Get().SetSelectedEntity(nullptr);
					m_Entities[EntityID]->DestroyEntity();
				}
				ImGui::PopStyleColor(3);
				/*
				if (NodeOpen)
				{
					ImGui::TreePop();
				}*/
            }

            ImGui::TreePop();
        }
	}
#endif
}