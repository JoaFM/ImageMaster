#include "Camera.h"
#include "Utils/IM_STD.h"

Camera::Camera() 
{
}

Camera::~Camera()
{
	TA_ERROR_WS(L"Camera got deleted");
}

IM_Math::Transfrom Camera::GetTransfrom() const
{
	return m_CameraData.Transform;
}

void Camera::UpdateCamera(Camera::CameraData CamData)
{
	m_CameraData = CamData;
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	// Camera Location rotation ------
	DirectX::XMFLOAT3 Poss(m_CameraData.Transform.Position.x, m_CameraData.Transform.Position.y, m_CameraData.Transform.Position.z);
	DirectX::XMFLOAT3 Up(0, 1, 0);

	const bool LookAtPoint = false;

	if (LookAtPoint)
	{
		DirectX::XMFLOAT3 LookPos(0, 0, 0);
		//#TODO : Can make the camera look at a world location
		return DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&Poss), DirectX::XMLoadFloat3(&LookPos), DirectX::XMLoadFloat3(&Up));
	}
	else
	{
		// assert(!XMVector3Equal(EyeDirection, XMVectorZero()));
		// assert(!XMVector3IsInfinite(EyeDirection));
		// assert(!XMVector3Equal(UpDirection, XMVectorZero()));
		// assert(!XMVector3IsInfinite(UpDirection));
	

		IM_Math::float3 forwa = m_CameraData.Transform.Forward() ;
		DirectX::XMFLOAT3 forwadsVec(forwa.x,forwa.y,forwa.z);
		
		return DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&Poss), DirectX::XMLoadFloat3(&forwadsVec), DirectX::XMLoadFloat3(&Up));
	}
}

DirectX::XMMATRIX Camera::GetProjectionMatrix()
{
	const float m_Otho_Near = -1000;
	if (m_CameraData.bIsPerspective)
	{
		return DirectX::XMMatrixPerspectiveFovLH(IM_Math::DegToRad(m_CameraData.FOV), m_CameraSizeX / m_CameraSizeY, m_CameraData.NearClip, m_CameraData.FarClip);
	}
	else
	{
		// camera to Clip
		return DirectX::XMMatrixOrthographicOffCenterLH(
			0,							//Minimum x-value of the view volume.
			(float)m_CameraSizeX,		//Maximum x-value of the view volume.
			(float)m_CameraSizeY,		//Minimum y-value of the view volume.
			0,
			m_CameraData.FarClip,//Maximum y-value of the view volume.
			m_Otho_Near				// Near CLip
								// Far Clip
		);
	}
}
