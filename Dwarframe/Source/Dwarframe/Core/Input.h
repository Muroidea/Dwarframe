#ifndef INPUT_H
#define INPUT_H

#include "Dwarframe/Core/KeyCodes.h"

namespace Dwarframe {

	class GameManager;
	class Window;

	enum EDwarframeKeyState : uint8
	{
		DF_KEY_PRESSED,
		DF_KEY_HOLD,
		DF_KEY,RELEASED
	};

	constexpr uint32 GetDwarframeKeyCodesEnumSizeIn64Ints()
	{
		constexpr uint32 SizeInBits = sizeof(uint64) * CHAR_BIT;
		return (EDwarframeKeyCodes::NUM_OF_ELEMENTS + SizeInBits - 1) / SizeInBits;
	}

	constexpr uint32 GetWordID(EDwarframeKeyCodes Key)
	{
		return Key / (sizeof(uint64) * CHAR_BIT);
	}
	
	class Input
	{
	public:
		inline bool IsKeyPressed(EDwarframeKeyCodes KeyCode)
		{
			return m_PressedKeys[GetWordID(KeyCode)] & ((uint64)1 << (KeyCode & 63)) && !(m_ReleasedKeys[GetWordID(KeyCode)] & ((uint64)1 << (KeyCode & 63)));
		}
		
		inline bool IsKeyReleased(EDwarframeKeyCodes KeyCode)
		{
			return !(m_PressedKeys[GetWordID(KeyCode)] & ((uint64)1 << (KeyCode & 63))) || m_ReleasedKeys[GetWordID(KeyCode)] & ((uint64)1 << (KeyCode & 63));
		}

		inline void GetMousePosition(int32& MousePosX, int32& MousePosY)
		{
			MousePosX = m_MousePosX;
			MousePosY = m_MousePosY;
		}
		
		inline void GetMousePositionChange(int32& MousePosChangeX, int32& MousePosChangeY)
		{
			MousePosChangeX = m_MouseChangeX;
			MousePosChangeY = m_MouseChangeY;
		}
		
		inline void GetMouseWheelChange(int32& MouseWheelChange)
		{
			MouseWheelChange = m_MouseWheelChange;
		}

		inline void RegisterKeyActionCallback(EDwarframeKeyCodes Code, EDwarframeKeyState State, std::function<void()> Func)
		{
			if (State == EDwarframeKeyState::DF_KEY_PRESSED)
			{
				m_OnKeyPressed[Code].push_back(Func);
			}
			else if (State == EDwarframeKeyState::DF_KEY_HOLD)
			{
				m_OnKeyHold[Code].push_back(Func);
			}
			else // State == EDwarframeKeyState::DF_KEY_RELEASED
			{
				m_OnKeyReleased[Code].push_back(Func);
			}
		}
		
		inline void RegisterMouseMovementCallback(std::function<void()> Func)
		{
			m_OnMouseMoved.push_back(Func);
		}
		
		inline void RegisterMouseWheelCallback(std::function<void()> Func)
		{
			m_OnWheelMoved.push_back(Func);
		}
		
		inline void UnregisterKeyActionCallback(EDwarframeKeyCodes Code, EDwarframeKeyState State, std::function<void()> Func)
		{
			/*
			if (State == EDwarframeKeyState::DF_KEY_PRESSED)
			{
				auto It = std::find(m_OnKeyPressed[Code].begin(), m_OnKeyPressed[Code].end(), Func);
				if (It != m_OnKeyPressed[Code].end())
				{
					m_OnKeyPressed[Code].erase(std::find(m_OnKeyPressed[Code].begin(), m_OnKeyPressed[Code].end(), Func));
				}
			}
			else if (State == EDwarframeKeyState::DF_KEY_HOLD)
			{
				auto It = std::find(m_OnKeyHold[Code].begin(), m_OnKeyHold[Code].end(), Func);
				if (It != m_OnKeyHold[Code].end())
				{
					m_OnKeyHold[Code].erase(std::find(m_OnKeyHold[Code].begin(), m_OnKeyHold[Code].end(), Func));
				}
			}
			else // State == EDwarframeKeyState::DF_KEY_RELEASED
			{
				auto It = std::find(m_OnKeyReleased[Code].begin(), m_OnKeyReleased[Code].end(), Func);
				if (It != m_OnKeyReleased[Code].end())
				{
					m_OnKeyReleased[Code].erase(std::find(m_OnKeyReleased[Code].begin(), m_OnKeyReleased[Code].end(), Func));
				}
			}
			*/
		}
		
		inline void UnregisterMouseMovementCallback(std::function<void()> Func)
		{
			/*
			auto It = std::find(m_OnMouseMoved.begin(), m_OnMouseMoved.end(), Func);
			if (It != m_OnMouseMoved.end())
			{
				m_OnMouseMoved.erase(std::find(m_OnMouseMoved.begin(), m_OnMouseMoved.end(), Func));
			}
			*/
		}
		
		inline void UnregisterMouseWheelCallback(std::function<void()> Func)
		{
			/*
			auto It = std::find(m_OnWheelMoved.begin(), m_OnWheelMoved.end(), Func);
			if (It != m_OnWheelMoved.end())
			{
				m_OnWheelMoved.erase(std::find(m_OnWheelMoved.begin(), m_OnWheelMoved.end(), Func));
			}
			*/
		}

	private:
		inline void SetMousePos(int32 MousePosX, int32 MousePosY)
		{
			m_MousePosX = MousePosX;
			m_MousePosY = MousePosY;
		}

		inline void MouseMoved(int32 ChangeX, int32 ChangeY, bool bAbsolute = false)
		{
			m_MousePosX += ChangeX;
			m_MousePosY += ChangeY;

			m_MouseChangeX = ChangeX;
			m_MouseChangeY = ChangeY;
			
			m_SingleActionsToPerform.push_back(&m_OnMouseMoved);
		}
		
		inline void WheelMoved(int32 Change)
		{
			m_MouseWheelPos += Change;
			m_MouseWheelChange = Change;
			m_SingleActionsToPerform.push_back(&m_OnWheelMoved);
		}

		inline void PressKey(EDwarframeKeyCodes KeyCode)
		{
			if (m_PressedKeys[GetWordID(KeyCode)] & ((uint64)1 << (KeyCode & 63)))
			{
				return;
			}

			m_SingleActionsToPerform.push_back(&m_OnKeyPressed[KeyCode]);
			m_HoldActionsToAdd[KeyCode] = &m_OnKeyHold[KeyCode];
			m_PressedKeys[GetWordID(KeyCode)] |= ((uint64)1 << (KeyCode & 63));
		}
		
		inline void ReleaseKey(EDwarframeKeyCodes KeyCode)
		{
			if (m_ReleasedKeys[GetWordID(KeyCode)] & ((uint64)1 << (KeyCode & 63)))
			{
				return;
			}

			m_HoldActionsToPerform.erase(KeyCode);
			m_SingleActionsToPerform.push_back(&m_OnKeyReleased[KeyCode]);
			m_ReleasedKeys[GetWordID(KeyCode)] |= ((uint64)1 << (KeyCode & 63));
		}

		inline void Update()
		{
			for (uint8 i = 0; i < GetDwarframeKeyCodesEnumSizeIn64Ints(); i++)
			{
				m_PressedKeys[i] &= ~m_ReleasedKeys[i];
			}
			memset(m_ReleasedKeys, 0, sizeof(m_ReleasedKeys));

			for (uint32 i = 0; i < m_SingleActionsToPerform.size(); i++)
			{
				for (uint32 j = 0; j < m_SingleActionsToPerform[i]->size(); j++)
				{
					m_SingleActionsToPerform[i]->at(j)();
				}
			}

			for (auto It : m_HoldActionsToPerform)
			{
				for (uint32 j = 0; j < It.second->size(); j++)
				{
					It.second->at(j)();
				}
			}

			m_SingleActionsToPerform.clear();

			m_HoldActionsToPerform.merge(std::move(m_HoldActionsToAdd));
			m_HoldActionsToAdd.clear();
		}

	private:
		int32 m_MousePosX, m_MousePosY;
		int32 m_MouseChangeX, m_MouseChangeY;

		int32 m_MouseWheelPos;
		int32 m_MouseWheelChange;

		uint64 m_PressedKeys[GetDwarframeKeyCodesEnumSizeIn64Ints()];
		uint64 m_ReleasedKeys[GetDwarframeKeyCodesEnumSizeIn64Ints()];
		
		std::array<std::vector<std::function<void()>>, EDwarframeKeyCodes::NUM_OF_ELEMENTS> m_OnKeyPressed;
		std::array<std::vector<std::function<void()>>, EDwarframeKeyCodes::NUM_OF_ELEMENTS> m_OnKeyHold;
		std::array<std::vector<std::function<void()>>, EDwarframeKeyCodes::NUM_OF_ELEMENTS> m_OnKeyReleased;
		
		std::vector<std::function<void()>> m_OnMouseMoved;
		std::vector<std::function<void()>> m_OnWheelMoved;

		std::vector<std::vector<std::function<void()>>*> m_SingleActionsToPerform;
		std::map<EDwarframeKeyCodes, std::vector<std::function<void()>>*> m_HoldActionsToPerform;
		std::map<EDwarframeKeyCodes, std::vector<std::function<void()>>*> m_HoldActionsToAdd;

	public:
		Input(const Input& Other) = delete; 
		Input& operator=(const Input& Other) = delete; 

		inline static Input& Get()
		{
			static Input Instance;

			return Instance;
		}

	private:
		Input() = default;

		friend Window;
		friend GameManager;
	};

}

#endif