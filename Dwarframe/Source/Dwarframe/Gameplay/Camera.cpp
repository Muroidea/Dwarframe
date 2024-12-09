#include "pch.h"
#include "Camera.h"

#include "Dwarframe/Core/Window.h"
#include "Dwarframe/Core/Input.h"

namespace Dwarframe {

	Camera::Camera()
	{
		Input& InputRef = Input::Get();
		InputRef.RegisterMouseMovementCallback(std::bind(&Camera::OnCameraRotate, this));

		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_TAB, EDwarframeKeyState::DF_KEY_PRESSED, std::bind(&Camera::OnCameraEnable, this));

		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_W, EDwarframeKeyState::DF_KEY_PRESSED, std::bind(&Camera::OnCameraMoved, this));
		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_W, EDwarframeKeyState::DF_KEY_HOLD, std::bind(&Camera::OnCameraMoved, this));

		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_S, EDwarframeKeyState::DF_KEY_PRESSED, std::bind(&Camera::OnCameraMoved, this));
		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_S, EDwarframeKeyState::DF_KEY_HOLD, std::bind(&Camera::OnCameraMoved, this));

		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_D, EDwarframeKeyState::DF_KEY_PRESSED, std::bind(&Camera::OnCameraMoved, this));
		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_D, EDwarframeKeyState::DF_KEY_HOLD, std::bind(&Camera::OnCameraMoved, this));

		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_A, EDwarframeKeyState::DF_KEY_PRESSED, std::bind(&Camera::OnCameraMoved, this));
		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_A, EDwarframeKeyState::DF_KEY_HOLD, std::bind(&Camera::OnCameraMoved, this));

		InputRef.RegisterKeyActionCallback(EDwarframeKeyCodes::DF_KEY_F10, EDwarframeKeyState::DF_KEY_PRESSED, std::bind(&Camera::OnCameraMoved, this));

		m_DefaultRight = XMVECTOR { 1.0f, 0.0f, 0.0f, 0.0f };
		m_DefaultUp = XMVECTOR { 0.0f, 1.0f, 0.0f, 0.0f };
		m_DefaultForward = XMVECTOR { 0.0f, 0.0f, 1.0f, 0.0f };
		
		m_CameraPosition = XMVECTOR { 0.0f, 8.0f, -8.0f, 0.0f };
		m_CameraForward = XMVECTOR { 0.0f, 0.0f, 1.0f, 0.0f };

		m_PositionChange = XMVECTOR { 0.0f, 0.0f, 0.0f, 0.0f };
		
		m_AspectRatio = static_cast<float32>(Window::Get().GetWindowWidth()) / static_cast<float32>(Window::Get().GetWindowHeight());
		Update(0.0f);
	}

	void Camera::OnResize(uint32 NewX, uint32 NewY)
	{
		m_AspectRatio = static_cast<float32>(Window::Get().GetWindowWidth()) / static_cast<float32>(Window::Get().GetWindowHeight());
	}

	void Camera::OnCameraMoved()
	{
		if (!bCameraEnabled)
		{
			return;
		}

		if (Input::Get().IsKeyPressed(EDwarframeKeyCodes::DF_KEY_W))
		{
			m_PositionChange += m_CameraForward;
		}
		if (Input::Get().IsKeyPressed(EDwarframeKeyCodes::DF_KEY_D))
		{
			m_PositionChange += m_CameraRight;
		}
		if (Input::Get().IsKeyPressed(EDwarframeKeyCodes::DF_KEY_S))
		{
			m_PositionChange -= m_CameraForward;
		}
		if (Input::Get().IsKeyPressed(EDwarframeKeyCodes::DF_KEY_A))
		{
			m_PositionChange -= m_CameraRight;
		}
		
		m_PositionChange = XMVectorScale(XMVector3Normalize(m_PositionChange), m_MovementSpeed);
		bHasChanged = true;
	}

	void Camera::OnCameraRotate()
	{
		if (bCameraEnabled && Input::Get().IsKeyPressed(EDwarframeKeyCodes::DF_MOUSE_LEFT_BUTTON))
		{
			int32 XChange, YChange;
			Input::Get().GetMousePositionDelta(XChange, YChange);

			m_Yaw += static_cast<float32>(XChange) * m_MouseSensitivity; 
			m_Pitch += static_cast<float32>(YChange) * m_MouseSensitivity;

			bHasChanged = true;
		}
	}

	void Camera::OnCameraEnable()
	{
		bCameraEnabled = !bCameraEnabled;
	}

	void Camera::Update(float32 DeltaTime)
	{
		m_AspectRatio = static_cast<float32>(Window::Get().GetWindowWidth()) / static_cast<float32>(Window::Get().GetWindowHeight());
		if (!bHasChanged)
		{
			return;
		}

		m_CameraPosition += m_PositionChange * DeltaTime;
		m_PositionChange = { 0.0f, 0.0f, 0.0f, 0.0f };

		XMMATRIX CameraRotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), 0);
		//m_TargetPosition = XMVector3TransformCoord(m_DefaultForward, CameraRotationMatrix );
		//m_TargetPosition = XMVector3Normalize(m_TargetPosition);
		
		// Free-Look Camera
		m_CameraRight = XMVector3TransformCoord(m_DefaultRight, CameraRotationMatrix);
		m_CameraForward = XMVector3TransformCoord(m_DefaultForward, CameraRotationMatrix);
		m_CameraUp = XMVector3Cross(m_CameraForward, m_CameraRight);

		//m_TargetPosition = m_CameraPosition + m_TargetPosition;

		/*
		// Calculate the new forward vector
        XMVECTOR NewForward;
		float32 YawSin, YawCos, PitchSin, PichCos;
		XMScalarSinCos(&YawSin, &YawCos, XMConvertToRadians(m_Yaw));
		XMScalarSinCos(&PitchSin, &PichCos, XMConvertToRadians(m_Pitch));
        NewForward.m128_f32[0] = YawCos * PichCos;
        NewForward.m128_f32[1] = PitchSin;
        NewForward.m128_f32[2] = YawSin * PichCos;

        m_CameraForward = XMVector3Normalize(NewForward);

        // Re-calculate the Right and Up vector
        m_CameraRight = XMVector3Normalize(XMVector3Cross(m_CameraForward, m_DefaultUp));
        m_CameraUp = XMVector3Normalize(XMVector3Cross(m_CameraRight, m_CameraForward));
		*/
	}

}