#include "pch.h"
#include "Editor.h"

#include "Dwarframe/Editor/WidgetsHelper.h"
#include "Dwarframe/Core/Window.h"
#include "Dwarframe/Renderer/DXContext.h"
#include "Dwarframe/Renderer/Pipeline/DescriptorHeap.h"
#include "Dwarframe/Resources/Managers/ResourceManager.h"
#include "Dwarframe/Resources/Material.h"
#include "Dwarframe/Resources/Mesh.h"
#include "Dwarframe/Gameplay/Entity.h"
#include "Dwarframe/Gameplay/World.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI/imgui_impl_dx12.h"

namespace Dwarframe {

#if WITH_EDITOR
	bool ImGUIEditor::Initialize()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		m_DescriptorHeap = new ShaderResourceDescriptorHeap();
		m_DescriptorHeap->Initialize(DXContext::Device(), 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(Window::Get().GetWindowHandle());
		ImGui_ImplDX12_Init(DXContext::Device(), DXContext::s_NumOfFrames,
			DXGI_FORMAT_R8G8B8A8_UNORM, m_DescriptorHeap->GetDescriptorHeap(), 
			m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 
			m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		return false;
	}

	void ImGUIEditor::Shutdown()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		m_DescriptorHeap->Shutdown();
	}

	void ImGUIEditor::Update(float DeltaTime)
	{
	}

	void ImGUIEditor::Render()
	{
		// Begin new ImGui frame
        ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Create docking space
		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

		const ImGuiViewport* Viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(Viewport->WorkPos);
		ImGui::SetNextWindowSize(Viewport->WorkSize);
		ImGui::SetNextWindowViewport(Viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Docking window", nullptr, WindowFlags);
		ImGui::PopStyleVar(2);


		ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiID DockspaceID = ImGui::GetID("Dock space");
		ImGui::DockSpace(DockspaceID, ImVec2(0.0f, 0.0f), DockspaceFlags);
		// Show demo windwo so we can look what we need
		ImGui::ShowDemoWindow();

		//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
		// Create rest of the windows
		RenderGeneralWindow();
		RenderContentWindow();
		RenderInspectorWindow();

		// End dock space window
		ImGui::End(); 

		// Render everything
        ImGui::Render();

		ID3D12DescriptorHeap* Descriptors[] = {
			m_DescriptorHeap->GetDescriptorHeap()
		};

		DXContext::Get().GetCommandList()->SetDescriptorHeaps(1, Descriptors);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DXContext::Get().GetCommandList());
	}

	void ImGUIEditor::Resize(uint32 Width, uint32 Height)
	{
		
	}

	void ImGUIEditor::RegisterEditorExtender(IEditorExtender* Extender)
	{
		m_RegisteredExtenders.push_back(Extender);
	}

	void ImGUIEditor::UnregisterEditorExtender(IEditorExtender* Extender)
	{
		m_RegisteredExtenders.erase(std::find(m_RegisteredExtenders.begin(), m_RegisteredExtenders.end(), Extender));
	}

	void ImGUIEditor::SetWorld(World* NewWorld)
	{ 
		// TODO: probably some checks are needed.
		m_CurrentWorld = NewWorld;
	}

	void ImGUIEditor::SetSelectedAsset(GameAsset* Extender)
	{
		m_SelectedAsset = Extender;
	}

	void ImGUIEditor::SetSelectedEntity(Entity* Extender)
	{
		m_SelectedEntity = Extender;
	}

	void ImGUIEditor::RenderGeneralWindow()
	{
		//ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		//ImGui::SetNextWindowSize(ImVec2(std::clamp(Width * 0.15f, 250.0f, 500.0f), Height));

		ImGuiWindowFlags LeftWindowFlags = 0;
		//LeftWindowFlags |= ImGuiWindowFlags_NoTitleBar;
		//LeftWindowFlags |= ImGuiWindowFlags_MenuBar;
		LeftWindowFlags |= ImGuiWindowFlags_NoMove;
		LeftWindowFlags |= ImGuiWindowFlags_NoCollapse;

		
		ImGui::Begin(EditorNames::LeftWindow.data());
		ImGui::SetWindowFontScale(1.4f);
		ImGuiID TestDockID = ImGui::GetWindowDockID();

		for (IEditorExtender* Extender : m_RegisteredExtenders)
		{
			if (Extender->Extends(EditorNames::LeftWindow))
			{
				Extender->RenderOptions();
			}
		}

		ImGuiIO& IO = ImGui::GetIO(); (void)IO;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / IO.Framerate, IO.Framerate);
        ImGui::End();
	}

	void ImGUIEditor::RenderContentWindow()
	{
		ImGuiWindowFlags ContentWindowFlags = 0;
		//ContentWindowFlags |= ImGuiWindowFlags_NoTitleBar;
		//ContentWindowFlags |= ImGuiWindowFlags_MenuBar;
		ContentWindowFlags |= ImGuiWindowFlags_NoMove;
		ContentWindowFlags |= ImGuiWindowFlags_NoCollapse;

		
		ImGui::Begin(EditorNames::ContentWindow.data());
		ImGui::SetWindowFontScale(1.4f);
		ImGuiID TestDockID = ImGui::GetWindowDockID();

		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

		const float TextLineHeight = ImGui::GetTextLineHeightWithSpacing();

		float WindowHeight = ImGui::GetWindowHeight();
		float FrameHeight = ImGui::GetFrameHeight();
		ImVec2 RegionSize = ImGui::GetContentRegionAvail();

        ImVec2 outer_size = ImVec2(0.0f, TextLineHeight * (RegionSize.y / TextLineHeight));
        if (ImGui::BeginTable("ContentTable", 4, flags, outer_size))
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("AssetPath", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            // Demonstrate using clipper for large vertical lists
			std::vector<ContentBasicInfo> ResourceList;
			for (IEditorExtender* Extender : m_RegisteredExtenders)
			{
				if (Extender->Extends(EditorNames::ContentWindow))
				{
					Extender->RenderResourceList(ResourceList);
				}
			}
			
            ImGuiListClipper Clipper;
            Clipper.Begin(ResourceList.size());
            while (Clipper.Step())
            {
                for (int RowID = Clipper.DisplayStart; RowID < Clipper.DisplayEnd; RowID++)
                {
                    ImGui::TableNextRow();
					
                    ImGui::TableSetColumnIndex(0);

					if (ImGui::Selectable(ResourceList[RowID].m_Name.data(), ResourceList[RowID].Asset == m_SelectedAsset, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, ImVec2(0, TextLineHeight)))
                    {
						m_SelectedAsset = ResourceList[RowID].Asset;
                    }
					
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_ContentAsset", &ResourceList[RowID].Asset, sizeof(ResourceList[RowID].Asset));
                        ImGui::Text("Drop on related resource.");
						ImGui::EndDragDropSource();
					}

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text(ResourceList[RowID].m_Type.data(), 1, RowID);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text(ResourceList[RowID].m_AssetPath.data(), 2, RowID);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text(std::to_string(ResourceList[RowID].m_Size).c_str(), 3, RowID);
                }
            }
            ImGui::EndTable();
        }

		ImGui::End();
	}

	void ImGUIEditor::RenderInspectorWindow()
	{
		ImGui::Begin(EditorNames::WorldOutlinerWindow.data());

		const std::vector<Entity*>& Entities = m_CurrentWorld->GetUnparentedEntities();

		if (ImGui::Button("Add new entity"))
        {
			// TODO: Change EntityCounter to something more clever...
			static uint32 EntityCounter = 0;
            m_CurrentWorld->CreateEntity("NewEntity_" + std::to_string(EntityCounter));
			EntityCounter++;
        }

		if (ImGui::TreeNode("Entities"))
		{
			uint32 GeneralID = 0;
			
			for (int32 EntityID = 0; EntityID < Entities.size(); EntityID++)
			{
				EditorHelper::DrawTreeNode(Entities[EntityID], GeneralID);
				GeneralID += Entities[EntityID]->GetNumOfChildren(true) + 1;
			}
			
            ImGui::TreePop();
        }

		ImGui::End();
		
		ImGui::Begin(EditorNames::PropertiesWindow.data());
		ImGui::SetWindowFontScale(1.4f);
		
		if (m_SelectedEntity)
		{
			m_SelectedEntity->RenderProperties();
		}
		else if (m_SelectedAsset)
		{
			m_SelectedAsset->RenderProperties();
		}

		ImGui::End();
	}

#endif
}