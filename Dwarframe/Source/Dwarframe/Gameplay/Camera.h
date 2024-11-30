#ifndef CAMERA_H
#define CAMERA_H

namespace Dwarframe {

	class Camera
	{
	public:
		Camera();
		~Camera() = default;

		inline XMMATRIX GetViewMatrix() const {
			return XMMatrixLookToLH(m_CameraPosition, m_CameraForward, m_CameraUp);
		}
		
		inline XMMATRIX GetProjectionMatrix() const {
			return XMMatrixPerspectiveFovLH(m_FieldOfView, m_AspectRatio, m_NearZ, m_FarZ);
		}

		void OnResize(uint32 NewX, uint32 NewY);
		void OnCameraMoved();
		void OnCameraRotate();
		void OnCameraEnable();
		
		void Update(float32 DeltaTime);

	private:
		XMVECTOR m_CameraPosition;
		XMVECTOR m_CameraForward;
		XMVECTOR m_CameraUp;
		XMVECTOR m_CameraRight;

		XMVECTOR m_DefaultUp;
		XMVECTOR m_DefaultForward;
		XMVECTOR m_DefaultRight;
		
		float32 m_Yaw = 0.0f;
		float32 m_Pitch = 0.0f;

		float32 m_FieldOfView = XMConvertToRadians(80.0f);
		float32 m_AspectRatio;
		float32 m_NearZ = 0.1f;
		float32 m_FarZ = 1000.0f;

		float32 m_MovementSpeed = 10.0f;
		float32 m_MouseSensitivity = 0.125f;

		bool bCameraEnabled = false;
		bool bHasChanged = true;
		XMVECTOR m_PositionChange;
	};

}

#endif
