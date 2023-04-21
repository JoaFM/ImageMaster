#pragma once
#include <Utils/IM_Math.h>
#include <DirectXMath.h>

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