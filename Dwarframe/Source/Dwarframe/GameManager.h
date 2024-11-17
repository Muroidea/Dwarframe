#ifndef GAMEMANGER_H
#define GAMEMANGER_H

#include "Core.h"
#include <vector>

namespace Dwarframe {

	class Window;
	class Clock;
	class DXDebugLayer;
	class DXContext;
	class GraphicsBridge;
	class ImGUIEditor;
	class ResourceManagerBase;
	class World;
	class RenderPass;

	class DWARFRAME_API GameManager
	{
	public:
		GameManager();
		virtual ~GameManager();

		virtual void Initialize();
		virtual void Shutdown();
		virtual void Run();

		virtual void LoadData();

		//inline void std::vector<RenderPass*> GetPasses() const { return m_AvailablePasses; }

	protected:

	private:

	protected:
		// Low-level systems
		Window* m_Window;
		Clock* m_SystemClock;
		DXDebugLayer* m_DXDebugLayer;
		GraphicsBridge* m_GraphicsBridge;
		DXContext* m_DXContext;

		std::vector<ResourceManagerBase*> m_ResourceManagers;

	#ifdef WITH_EDITOR
		ImGUIEditor* m_Editor;
	#endif

		// Gameplay related elements
		World* m_CurrentWorld;
		std::vector<RenderPass*> m_AvailablePasses;
	};

	GameManager* CreateGameManager();
}

#endif // !GAMEMANGER_H