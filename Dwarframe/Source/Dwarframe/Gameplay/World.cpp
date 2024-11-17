#include "pch.h"
#include "World.h"

#include "Dwarframe/Gameplay/Entity.h"
#include "Dwarframe/Renderer/RenderPasses/RenderPass.h"

#include "ImGUI/imgui.h"

namespace Dwarframe {

    World::World()
    {
    }

    void World::SetRenderPasses(std::vector<RenderPass*>* RenderPasses)
    {
        m_AvailablePasses = RenderPasses;
    }

    void World::AddNewGameObject(Entity* NewEntity)
    {
        if (NewEntity)
		{
			m_AddedEntities.push_back(NewEntity);
        }
    }

	void World::AddRenderable(Renderable* NewRenderable)
	{
        if (NewRenderable)
		{
			m_AddRenderable.push_back(NewRenderable);
        }
	}

	void World::Update(float Delta)
    {
        for (Entity* Ent : m_AddedEntities)
        {
            // TODO: Rethink - some extra logic like BeginPlay() before adding to world?
			m_Entities.push_back(Ent);
        }
        m_AddedEntities.clear();

        for (Renderable* Ren : m_AddRenderable)
		{
            // TODO: Adding to proper passes when will be more of them
			m_AvailablePasses->operator[](0)->AddRenderable(Ren);
        }
        m_AddRenderable.clear();
    }

    void World::RenderGameObjects()
	{
        if (ImGui::Button("AddNewGameObject"))
        {
            AddNewGameObject(new Entity("NewGameObject", this));
        }
        
		if (ImGui::TreeNode("GameObjects"))
        {
            static ImGuiTreeNodeFlags BaseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
            static bool TestDragAndDrop = false;
            
            for (int32 EntityID = 0; EntityID < m_Entities.size(); EntityID++)
            {
                ImGuiTreeNodeFlags NodeFlags = BaseFlags;
                const bool IsSelected = SelectedEntity == EntityID;

                if (IsSelected) 
                { 
                    NodeFlags |= ImGuiTreeNodeFlags_Selected; 
                }

                NodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                ImGui::TreeNodeEx((void*)(intptr_t)EntityID, NodeFlags, m_Entities[EntityID]->GetName().data(), EntityID);

                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                {
                    ImGUIEditor::Get().SetSelected(m_Entities[EntityID]);
                }

                if (TestDragAndDrop && ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                    ImGui::Text("This is a drag and drop source");
                    ImGui::EndDragDropSource();
                }
            }

            ImGui::TreePop();
        }
	}
}