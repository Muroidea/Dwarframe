#include "pch.h"
#include "Material.h"

#include "Dwarframe/Helpers/ShaderCompiler.h"
#include "Dwarframe/Renderer/GPUResources/Shader.h"

#include "ImGUI/imgui.h"


namespace Dwarframe {

	Material::Material(std::string AssetPath, std::string AssetName)
		: GameAsset(AssetPath, AssetName)
	{
		SetRenderingPipelineType(ERenderingPipelineType::MeshShaderPipeline);
	}

	bool Material::LoadAsset()
	{
		std::filesystem::path AssetPath { m_AssetPath };
		if (!std::filesystem::exists(AssetPath))
		{
			// TODO: Nothing to load (no file exist) or wrong path
			return false;
		}
		
		std::ifstream InFile (AssetPath, std::ios::in);

		if (!InFile.is_open())
		{
			return false;
		}
		
		std::string Temp;
		InFile >> Temp;
		m_Pipeline = static_cast<ERenderingPipelineType>(std::stoi(Temp));
		for (std::string ShaderType, ShaderPath, EntryPoint; InFile >> ShaderType >> ShaderPath >> EntryPoint;)
		{
			EShaderType Type = static_cast<EShaderType>(std::stoi(ShaderType));
			for (Shader* Shad : m_ShaderList)
			{
				if (Shad->m_ShaderType == Type)
				{
					//memcpy(Shad->m_Path.data(), ShaderPath.data() + 1, ShaderPath.size() - 1);
					//memcpy(Shad->m_EntryPoint.data(), EntryPoint.data() + 1, EntryPoint.size() - 1);
					ShaderPath.copy(Shad->m_Path.data(), ShaderPath.size() - 1, 1);
					EntryPoint.copy(Shad->m_EntryPoint.data(), EntryPoint.size() - 1, 1);
					ShaderCompiler::Compile(Shad);
					break;
				}
			}
		}
		ShaderCompiler::CompileRootSignature(m_Pipeline, m_ShaderList, &m_RootSignature);

		return true;
	}

	bool Material::SaveAsset()
	{
		std::filesystem::path AssetPath { m_AssetPath };
		std::ofstream OutFile (AssetPath, std::ios::out);

		if (!OutFile.is_open())
		{
			return false;
		}

		OutFile << std::to_string((uint8)m_Pipeline) << " \n";
		for (Shader* Shad : m_ShaderList)
		{
			OutFile << std::to_string((uint8)Shad->m_ShaderType) << "\n";
			OutFile << "." << Shad->m_Path.c_str() << "\n";
			OutFile << "." << Shad->m_EntryPoint.c_str() << "\n";
		}

		return true;
	}
	
	const void* Material::GetShaderBinary(EShaderType ShaderType) const
	{
		for (Shader* Elem : m_ShaderList)
		{
			if (Elem->GetShaderType() == ShaderType)
			{
				return Elem->GetShaderBinary();
			}
		}

		return nullptr;
	}

	uint64 Material::GetShaderSize(EShaderType ShaderType) const
	{
		for (Shader* Elem : m_ShaderList)
		{
			if (Elem->GetShaderType() == ShaderType)
			{
				return Elem->GetShaderSize();
			}
		}

		return 0;
	}

	void Material::SetRenderingPipelineType(ERenderingPipelineType NewPipeline)
	{
		if (m_Pipeline == NewPipeline)
		{
			return;
		}

		m_Pipeline = NewPipeline;

		EShaderType *List;
		uint32_t NumOfShaders;
		GetPipelineSupportedShaders(NewPipeline, List, NumOfShaders);

		m_ShaderList.clear();
		for (uint32_t ID = 0; ID < NumOfShaders; ID++)
		{
			m_ShaderList.push_back(new Shader(ShaderTypeToString(List[ID])));
			m_ShaderList.back()->SetShaderType(List[ID]);
		}
	}

	void Material::RenderOptions()
	{
		ImGui::Text("Material info:");
		
		ImGui::Spacing();
		ImGui::Text("Choose pipeline:");
        if (ImGui::RadioButton("Classic", m_Pipeline == ERenderingPipelineType::ClassicPipeline)) { SetRenderingPipelineType(ERenderingPipelineType::ClassicPipeline); } ImGui::SameLine();
        if (ImGui::RadioButton("Mesh shader", m_Pipeline == ERenderingPipelineType::MeshShaderPipeline)) { SetRenderingPipelineType(ERenderingPipelineType::MeshShaderPipeline); } ImGui::SameLine();
        if (ImGui::RadioButton("Compute", m_Pipeline == ERenderingPipelineType::ComputePipeline)) { SetRenderingPipelineType(ERenderingPipelineType::ComputePipeline); }

		static bool bUseSingleFile = false; 
		ImGui::Checkbox("Use single shader file.", &bUseSingleFile);

		if (bUseSingleFile)
		{
			if (ImGui::InputText("Path", m_ShaderList[0]->GetPath().data(), m_ShaderList[0]->GetPath().size(), ImGuiInputTextFlags_None))
			{
				for (uint32 ShaderID = 1; ShaderID < m_ShaderList.size(); ShaderID++)
				{
					m_ShaderList[ShaderID]->GetPath() = m_ShaderList[0]->GetPath();
				}
			}
		}

		std::vector<const char*> Items;
		Items.reserve(EShaderType::NumOfElements);
		for (uint8_t TypeID = 0; TypeID < EShaderType::NumOfElements; TypeID++)
		{
			Items.emplace_back(ShaderTypeToString(static_cast<EShaderType>(TypeID)));
		}

		uint32_t ImGuiID = 0;
		for (Shader* Shader : m_ShaderList)
		{
			ImGui::Spacing();
			ImGui::Separator();

			const char* PreviewValue = Items[Shader->GetShaderType()];
			ImGui::PushID(ImGuiID);
			if (ImGui::BeginCombo("ShaderType", PreviewValue, 0))
			{
				for (int ID = 0; ID < Items.size(); ID++)
				{
					const bool IsSelected = (Shader->GetShaderType() == ID);
					if (ImGui::Selectable(Items[ID], IsSelected))
					{
						Shader->SetShaderType(static_cast<EShaderType>(ID));
					}

					if (IsSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::InputText("EntryPoint", Shader->GetEntryPoint().data(), Shader->GetEntryPoint().size(), ImGuiInputTextFlags_None);
			if (!bUseSingleFile)
			{
				ImGui::InputText("Path", Shader->GetPath().data(), Shader->GetPath().size(), ImGuiInputTextFlags_None);
			}

			if (ImGui::Button("Load"))
			{
				ShaderCompiler::Load(Shader);
			}

			ImGui::SameLine();
			if (ImGui::Button("Compile"))
			{
				ShaderCompiler::Compile(Shader);
			}
			ImGui::PopID();
			ImGui::Spacing();

			ImGuiID++;
		}
		ImGui::Separator();

		
		if (ImGui::Button("Build Root Signature"))
		{
			ShaderCompiler::CompileRootSignature(m_Pipeline, m_ShaderList, &m_RootSignature);
		}

		if (ImGui::Button("Edit Root Signature Manually"))
		{
			
		}
		
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		if (ImGui::Button("SaveAsset"))
		{
			SaveAsset();
		}
	}

}
