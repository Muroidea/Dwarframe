#include "pch.h"
#include "Clock.h"

namespace Dwarframe {
	void Clock::Initialize()
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);
		QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentTime);
		m_PreviousTime = m_CurrentTime;

		m_InversedFrequency = 1.0f / m_Frequency;
	}

	void Clock::Shutdown()
	{
		m_CurrentTime = 0;
		m_PreviousTime = 0;

		m_Frequency = 0;
		m_InversedFrequency = 0.0f;
	}

	void Clock::Tick()
	{
		m_PreviousTime = m_CurrentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentTime);
		
		m_DeltaTime = m_CurrentTime - m_PreviousTime;
		m_ElapsedTime += m_DeltaTime;
	}

	Clock::Clock()
		: m_DeltaTime(0), m_ElapsedTime(0), m_PreviousTime(0), m_CurrentTime(0), m_Frequency(0), m_InversedFrequency(0.0f)
	{
	}

}
