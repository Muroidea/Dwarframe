#ifndef WIDGETS_HELPER_H
#define WIDGETS_HELPER_H

#include "Dwarframe/Gameplay/Entity.h"
#include "Dwarframe/Math/Transform.h"
#include "Dwarframe/Resources/GameAsset.h"
#include "ImGUI/imgui.h"

#include <limits>

namespace Dwarframe {

#if WITH_EDITOR
	template <typename Derived>
	concept GameAssetDerived = std::is_base_of<GameAsset, Derived>::value;

	// TODO: Rethink it. Maybe it might be better if it's done with preprocessor
	class EditorHelper
	{
	public:
		static inline void DrawTransform(Entity& InputEntity)
		{
			constexpr float Max = std::numeric_limits<float>::max();

			static bool LockScale = false;

			XMVECTOR Translation = InputEntity.GetTranslation();
			XMVECTOR Scale = InputEntity.GetScale();
			XMVECTOR Rotation = InputEntity.GetRotationEuler();
			/*
			XMVECTOR AxisRotation;
			float Angle;
			XMQuaternionToAxisAngle(&AxisRotation, &Angle, Rotation);
			AxisRotation.m128_f32[3] = 0.0f;
			AxisRotation = XMVectorScale(AxisRotation, Angle);
			//AxisRotation = XMVectorScale(AxisRotation, Angle * (180.0f / XM_PI));
			*/
			if (ImGui::DragFloat3("Translation", (float*)&Translation, 0.01f, -Max, Max))
			{
				InputEntity.SetTranslation(Translation);
			}

			XMVECTOR ScaleOld = Scale;
			if (ImGui::DragFloat3("Scale", (float*)&Scale, 0.01f, 0.0f, Max))
			{
				if (LockScale)
				{
					ScaleOld = XMVectorSubtract(Scale, ScaleOld);

					if (ScaleOld.m128_f32[0] != 0)
					{
						Scale.m128_f32[1] += ScaleOld.m128_f32[0];
						Scale.m128_f32[2] += ScaleOld.m128_f32[0];
					}
					else if (ScaleOld.m128_f32[1] != 0)
					{
						Scale.m128_f32[0] += ScaleOld.m128_f32[1];
						Scale.m128_f32[2] += ScaleOld.m128_f32[1];
					}
					else if (ScaleOld.m128_f32[2] != 0)
					{
						Scale.m128_f32[0] += ScaleOld.m128_f32[2];
						Scale.m128_f32[1] += ScaleOld.m128_f32[2];
					}
				}

				InputEntity.SetScale(Scale);
			}

			ImGui::SameLine();
			ImGui::Checkbox("Lock", &LockScale);

			if (ImGui::DragFloat3("Rotation", (float*)&Rotation, 0.01f, -Max, Max))
			{
				//AxisRotation = XMVectorScale(AxisRotation, (XM_PI / 180.0f));
				//Rotation = XMQuaternionRotationRollPitchYawFromVector(AxisRotation);
				InputEntity.SetRotationEuler(Rotation);
			}
		}
		
		static inline void DrawTreeNode(Entity* InEntity, uint32 ID = 0)
		{
			bool IsSelected = InEntity == ImGUIEditor::Get().GetSelectedEntity();
			bool HasChildren = InEntity->HasChildren();
			ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

			if (IsSelected)
			{
				NodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			if (!HasChildren)
			{
				NodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			}

			ImGui::SetNextItemAllowOverlap();
			bool NodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)ID, NodeFlags, InEntity->GetName().data());

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				ImGUIEditor::Get().SetSelectedEntity(InEntity);
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("_Entity", &InEntity, sizeof(Entity));
				ImGui::Text("This is a drag and drop source");
				ImGui::EndDragDropSource();
			}

			Entity* Temp;
			EditorHelper::AddEntityDragAndDropTarget(Temp, [&InEntity, &Temp]() {
				InEntity->AddChild(Temp);
			});

			ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Delete").x - 50.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

			std::string Name = "Delete##" + std::to_string(ID);
			if (ImGui::SmallButton(Name.c_str()))
			{
				ImGUIEditor::Get().SetSelectedEntity(nullptr);
				InEntity->DestroyEntity();
			}

			if (NodeOpen && HasChildren)
			{
				for (Entity* ChildEnt : InEntity->GetChildren())
				{
					if (!ChildEnt->IsMarkedToDestroy())
					{
						DrawTreeNode(ChildEnt, ++ID);
					}
				}

				ImGui::TreePop();
			}

			ImGui::PopStyleColor(3);
		}

		// TODO: Change std::function to Delegate
		template <GameAssetDerived ExpectedType>
		static inline void AddAssetDragAndDropTarget(ExpectedType*& Result, std::function<void()> Behaviour)
		{
			if (ImGui::BeginDragDropTarget())
			{
				// TODO: Change "_ContentAsset" to something relevant to asset type?
				if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("_ContentAsset"))
				{
					GameAsset* Asset = *static_cast<GameAsset**>(Payload->Data);

					if (Asset && TypeID::Value<ExpectedType>() == Asset->GetID())
					{
						Result = static_cast<ExpectedType*>(Asset);
						Behaviour();
					}
				}

				ImGui::EndDragDropTarget();
			}
		}

		// TODO: Change std::function to Delegate
		static inline void AddEntityDragAndDropTarget(Entity*& Result, std::function<void()> Behaviour)
		{
			if (ImGui::BeginDragDropTarget())
			{
				// TODO: Change "_Entity" to easier and move it to somewhere with all names
				if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("_Entity"))
				{
					Entity* Ent = *static_cast<Entity**>(Payload->Data);

					if (Ent)
					{
						Result = Ent;
						Behaviour();
					}
				}

				ImGui::EndDragDropTarget();
			}
		}
	};

#endif
}

#endif // !WIDGETS_HELPER_H
