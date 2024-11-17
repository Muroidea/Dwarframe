#include "pch.h"
#include "GameManager.h"

#define NOMINMAX
#include "Common/Clock.h"
#include "Window.h"
#include "Dwarframe/Renderer/RenderPasses/BaseRenderPass.h"
#include "Dwarframe/Renderer/Debug/DXDebugLayer.h"
#include "Dwarframe/Renderer/HardwareBridge/GraphicsBridge.h"
#include "Dwarframe/Resources/Managers/MeshManager.h"
#include "Dwarframe/Resources/Managers/TextureManager.h"
#include "Dwarframe/Resources/Managers/MaterialManager.h"
#include "Dwarframe/Gameplay/World.h"
#include "Renderer/DXContext.h"
#include "Dwarframe/Editor/Editor.h"

namespace Dwarframe {

	GameManager::GameManager()
        : m_Window(nullptr), m_DXDebugLayer(nullptr), m_DXContext(nullptr)
	{
	}

	GameManager::~GameManager()
	{
	}

	void GameManager::Initialize()
	{
		Paths::InitializePaths();

		m_Window = &Window::Get();
        m_Window->Initialize();

		m_SystemClock = &Clock::Get();
		m_SystemClock->Initialize();

        m_DXDebugLayer = &DXDebugLayer::Get();
        m_DXDebugLayer->Initialize();

		m_GraphicsBridge = &GraphicsBridge::Get();
		m_GraphicsBridge->Initialize();

        m_DXContext = &DXContext::Get();
        m_DXContext->Initialize();
		
		m_AvailablePasses.push_back(new BaseRenderPass());

	#ifdef WITH_EDITOR
		m_Editor = &ImGUIEditor::Get();
		m_Editor->Initialize();
	#endif
	
		m_ResourceManagers.push_back(new MeshManager());
		m_ResourceManagers.back()->Initialize();

		m_ResourceManagers.push_back(new TextureManager());
		m_ResourceManagers.back()->Initialize();
		
		m_ResourceManagers.push_back(new MaterialManager());
		m_ResourceManagers.back()->Initialize();
		
		m_CurrentWorld = new World();
		m_CurrentWorld->SetRenderPasses(&m_AvailablePasses);

	#ifdef WITH_EDITOR
		m_Editor->SetWorld(m_CurrentWorld);

		for (auto& Manager : m_ResourceManagers)
		{
			m_Editor->RegisterEditorExtender(Manager);
		}
	#endif

		m_GraphicsBridge->InitializeSwapChain(m_Window->GetWindowHandle(), m_Window->GetWindowWidth(), m_Window->GetWindowHeight());
		m_Window->m_OnResizeFunction = std::bind(&GraphicsBridge::ResizeSwapChainsBuffers, m_GraphicsBridge, std::placeholders::_1, std::placeholders::_2);
	}

	void GameManager::Shutdown()
	{
		delete m_CurrentWorld;
		m_CurrentWorld = nullptr;

	#ifdef WITH_EDITOR
		m_Editor->Shutdown();
		m_Editor = nullptr;
	#endif

		for (ResourceManagerBase* Manager : m_ResourceManagers)
		{
			Manager->Shutdown();
			delete Manager;
			Manager = nullptr;
		}
		m_ResourceManagers.clear();

        m_DXContext->Shutdown();
        m_DXContext = nullptr;
		
		m_GraphicsBridge->Shutdown();
		m_GraphicsBridge = nullptr;

        m_DXDebugLayer->Shutdown();
        m_DXDebugLayer = nullptr;

		m_SystemClock->Shutdown();
		m_SystemClock = nullptr;

        m_Window->Shutdown();
        m_Window = nullptr;
	}
    
	void GameManager::LoadData()
	{
	}

	void GameManager::Run()
	{
		m_DXContext->LoadData();
		float Delta;

		while (m_Window->HandleEvents())
		{
			m_SystemClock->Tick();
			Delta = m_SystemClock->GetDeltaTime();

			ID3D12GraphicsCommandList6* CommandList = m_DXContext->GetCommandList();
            m_DXContext->ResetCommandList();
			m_GraphicsBridge->BeginFrame(CommandList);
			
			m_DXContext->Update(Delta);
			m_CurrentWorld->Update(Delta);

			for (RenderPass* Pass : m_AvailablePasses)
			{
				Pass->Update();
			}

			for (RenderPass* Pass : m_AvailablePasses)
			{
				Pass->Render();
			}

	#ifdef WITH_EDITOR
			m_Editor->Render();
	#endif

			m_GraphicsBridge->EndFrame(CommandList);
			m_DXContext->DispatchCommandQueue();
			m_GraphicsBridge->PresentAndSwapBuffers();
		};

		m_DXContext->Flush();
	}
}
