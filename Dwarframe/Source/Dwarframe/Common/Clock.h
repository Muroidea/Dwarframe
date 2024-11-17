#ifndef CLOCK_H
#define CLOCK_H

namespace Dwarframe {

	class Clock
	{
	public:
		void Initialize();
		void Shutdown();

		void Tick();

		float GetDeltaTime() const { return m_DeltaTime * m_InversedFrequency; }
		float GetElapsedTime() const { return m_ElapsedTime * m_InversedFrequency; }

	private:
		int64 m_DeltaTime;
		int64 m_ElapsedTime;

		int64 m_PreviousTime;
		int64 m_CurrentTime;

		int64 m_Frequency;
		float m_InversedFrequency;
		
	public:
		Clock(const Clock& Other) = delete; 
		Clock& operator=(const Clock& Other) = delete; 

		inline static Clock& Get()
		{
			static Clock Instance;

			return Instance;
		}

	private:
		Clock();
	};

}

#endif // !CLOCK_H
