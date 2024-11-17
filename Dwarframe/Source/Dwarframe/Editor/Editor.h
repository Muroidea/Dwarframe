#ifndef EDITOR_H
#define EDITOR_H

namespace Dwarframe {

	class ShaderResourceDescriptorHeap;
	class ResourceManagerBase;
	class IEditorExtender;
	class IResourceInspectorExtender;
	class World;

	class EditorNames
	{
	public:
		inline static const std::string LeftWindow = "GeneralWindow";
		inline static const std::string ContentWindow = "ContentWindow";
		inline static const std::string RightWindow = "InspectorWindow";
	};

	class ImGUIEditor
	{
	public:

        bool Initialize();
		void Shutdown();
        void Update(float DeltaTime);
        void Render();

        void Resize(uint32 Width, uint32 Height);

		void RegisterEditorExtender(IEditorExtender* Extender);
		void SetWorld(World* NewWorld);
		void SetSelected(IResourceInspectorExtender* Extender);

		ImGUIEditor(const ImGUIEditor& Other) = delete;
		ImGUIEditor& operator=(const ImGUIEditor& Other) = delete;

		inline static ImGUIEditor& Get()
		{
			static ImGUIEditor Instance;

			return Instance;
		}

	private:
		ImGUIEditor() = default;

		void RenderGeneralWindow();
		void RenderContentWindow();
		void RenderInspectorWindow();

	private:
        ShaderResourceDescriptorHeap* m_DescriptorHeap;
		std::vector<IEditorExtender*> m_RegisteredExtenders;
		//std::vector<std::string> m_RegisteredResourcesWindow;
		
		World* m_CurrentWorld;

		uint32 m_SelectedContentRow = std::numeric_limits<uint32>::max();
		IResourceInspectorExtender* m_SelectedExtender = nullptr;
	};

}

#endif // !EDITOR_H
