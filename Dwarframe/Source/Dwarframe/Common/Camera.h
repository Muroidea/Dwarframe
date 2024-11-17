#ifndef CAMERA_H
#define CAMERA_H

using namespace DirectX;

namespace Dwarframe {


	class Camera 
	{
	public:
		Camera() = default;
		Camera(XMVECTOR Up) 
			: m_Up(Up) {}
		~Camera() = default;

		inline XMMATRIX GetViewMatrix()
		{
			return XMMatrixLookAtLH(m_Position, m_Target, m_Up);
		}

		inline void SetPosition(XMVECTOR NewPosition) { m_Position = NewPosition; }
		inline void SetTarget(XMVECTOR NewTarget) { m_Target = NewTarget; }
		inline void SetFront(XMVECTOR NewFront) { m_Front = NewFront; }
		inline void SetUp(XMVECTOR NewUp) { m_Up = NewUp; }

	private:
		XMVECTOR m_Position;
		XMVECTOR m_Target;
		XMVECTOR m_Front;
		XMVECTOR m_Up;

		float m_FOV = 90.0f;
		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
	};

}

#endif // !CAMERA_H
