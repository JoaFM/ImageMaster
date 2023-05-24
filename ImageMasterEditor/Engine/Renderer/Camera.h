#pragma once
#include <Utils/IM_Math.h>
#include <DirectXMath.h>
#include "CameraData.h"


class Camera 
{

public:
	
	Camera();
	~Camera();

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