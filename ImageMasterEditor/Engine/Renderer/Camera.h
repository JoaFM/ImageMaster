#pragma once
#include <Utils/IM_Math.h>
#include <DirectXMath.h>


class Camera 
{

public:
	
	Camera();
	~Camera();
public:
	
	// Sync This with camera.cs
	struct CameraData
	{
		bool bIsPerspective;
		float FOV;
		float NearClip;
		float FarClip;
		IM_Math::Transfrom Transform;
		//TAMath::float3 CameraDirection;

		CameraData() :
			bIsPerspective(false),
			FOV(45),
			NearClip(0.01f),
			FarClip(1000),
			Transform(IM_Math::Transfrom(IM_Math::float3(0, 0, -4), IM_Math::float3(1, 1, 1), IM_Math::Quaternion(0, 0, 0, 1)))
			//CameraDirection(0,0,1)

		{
		}
	};

public:
	void UpdateCamera(CameraData CamData);
	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetProjectionMatrix();
	void SetCameraSizeX(float NewSizeX) { m_CameraSizeX = NewSizeX; }
	void SetCameraSizeY(float NewSizeY) { m_CameraSizeY = NewSizeY; }
	IM_Math::Transfrom GetTransfrom() const;


private: 
	CameraData m_CameraData;
	float m_CameraSizeX = 1000;
	float m_CameraSizeY = 1000;

};