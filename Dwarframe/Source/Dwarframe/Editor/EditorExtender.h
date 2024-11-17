#ifndef EDITOR_EXTENDER_H
#define EDITOR_EXTENDER_H

namespace Dwarframe {

	class GameAsset;

	struct ContentBasicInfo
	{
		std::string_view m_Name;
		std::string_view m_Type;
		std::string_view m_AssetPath;
		uint32 m_Size;
		GameAsset* Asset;
		IResourceInspectorExtender* m_InspectorExtender;
	};

	class IEditorExtender
	{
	public:
	#ifdef WITH_EDITOR
		virtual bool Extends(std::string ElementName) = 0;

		virtual void RenderOptions() {};
		virtual void RenderResourceList(std::vector<ContentBasicInfo>& ResourceList) {};
	#endif
	};

	class IResourceInspectorExtender
	{
	public:
	#ifdef WITH_EDITOR
		virtual void RenderOptions() = 0;
	#endif
	};
}

#endif // !EDITOR_EXTENDER_H