#ifndef EDITOR_H
#define EDITOR_H

namespace Dwarframe {

	class ShaderResourceDescriptorHeap;
	class ResourceManagerBase;
	class IEditorExtender;
	class IResourceInspectorExtender;
	class World;

	class Entity;
	class GameAsset;

#if WITH_EDITOR
	class EditorNames
	{
	public:
		inline static const std::string_view LeftWindow = "GeneralWindow";
		inline static const std::string_view ContentWindow = "ContentWindow";
		inline static const std::string_view WorldOutlinerWindow = "WorldOutliner";
		inline static const std::string_view PropertiesWindow = "PropertiesWindow";
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
		void UnregisterEditorExtender(IEditorExtender* Extender);

		void SetWorld(World* NewWorld);

		void SetSelectedAsset(GameAsset* Extender);
		void SetSelectedEntity(Entity* Extender);

		GameAsset* GetSelectedAsset() const { return m_SelectedAsset; }
		Entity* GetSelectedEntity() const { return m_SelectedEntity; }

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
        ShaderResourceDescriptorHeap* m_DescriptorHeap = nullptr;
		std::vector<IEditorExtender*> m_RegisteredExtenders {};
		//std::vector<std::string> m_RegisteredResourcesWindow;
		
		World* m_CurrentWorld = nullptr;

		// Asset Window info
		GameAsset* m_SelectedAsset = nullptr;

		// World Outliner info
		Entity* m_SelectedEntity = nullptr;
	};

#endif
}

#endif // !EDITOR_H
