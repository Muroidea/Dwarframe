#include "pch.h"
#include "Shader.h"

namespace Dwarframe {


	Shader::Shader(std::string_view ShaderName)
	{
		m_Compiled = false;

		static std::filesystem::path ShaderDir;
		if (ShaderDir.empty())
		{
			wchar_t ModuleFileName[MAX_PATH];
			GetModuleFileName(nullptr, ModuleFileName, MAX_PATH);

			ShaderDir = ModuleFileName;
			ShaderDir.remove_filename();
		}

		std::ifstream ShaderIn(ShaderDir / ShaderName, std::ios::binary);
		if (ShaderIn.is_open())
		{
			ShaderIn.seekg(0, std::ios::end);
			m_Size = ShaderIn.tellg();
			ShaderIn.seekg(0, std::ios::beg);
			m_Data = malloc(m_Size);
			if (m_Data)
			{
				ShaderIn.read((char*)m_Data, m_Size);
			}
		}

		m_Path.resize(256);
		m_EntryPoint.resize(256);
	}

	Shader::~Shader()
	{
		if (m_Data)
		{
			free(m_Data);
			m_Data = nullptr;
			m_Size = 0;
		}
	}

}