#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <DirectXMath.h>

using namespace DirectX;

namespace Dwarframe {

	struct Transform
	{
	public:
		Transform()
		{
			m_Translation = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
			m_RotationEuler = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_Scale = { 1.0f, 1.0f, 1.0f, 0.0f };
		}

		inline XMVECTOR GetTranslation() const
		{
			return m_Translation;
		}

		inline XMVECTOR GetRotation() const
		{
			return m_Rotation;
		}

		inline XMVECTOR GetRotationEuler() const
		{
			return m_RotationEuler;
		}

		inline XMVECTOR GetScale() const
		{
			return m_Scale;
		}

		inline void SetTranslation(XMVECTOR NewTranslation)
		{
			m_Translation = NewTranslation;
		}

		inline void SetRotation(XMVECTOR NewRotation)
		{
			m_Rotation = NewRotation;
		}

		inline void SetRotationEuler(XMVECTOR NewRotation)
		{
			m_RotationEuler = NewRotation;
		}

		inline void SetScale(XMVECTOR NewScale)
		{
			m_Scale = NewScale;
		}

		inline void Translate(XMVECTOR Translation)
		{
			m_Translation = XMVectorAdd(m_Translation, Translation);
		}

		inline void Rotate(XMVECTOR Rotation)
		{
			m_Rotation = XMQuaternionMultiply(m_Rotation, Rotation);
		}

		inline void RotateEuler(XMVECTOR Rotation)
		{
			m_RotationEuler = XMVectorAdd(m_Rotation, Rotation);
		}

		inline void Scale(XMVECTOR Scale)
		{
			m_Scale = XMVectorAdd(m_Scale, Scale);
		}

		inline XMMATRIX GetTransform()
		{
			XMMATRIX Matrix = XMMatrixScalingFromVector(m_Scale);
			//Matrix *= XMMatrixRotationQuaternion(m_Rotation);
			Matrix *= XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(m_RotationEuler));
			Matrix *= XMMatrixTranslationFromVector(m_Translation);

			return Matrix;
		}

	private:
		XMVECTOR m_Translation;
		XMVECTOR m_Rotation; // Stored as quat
		XMVECTOR m_RotationEuler; // Stored as euler angles
		XMVECTOR m_Scale;
	};
}

#endif // !TRANSFORM_H