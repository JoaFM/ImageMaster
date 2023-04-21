#pragma once
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>
#include "MainApp/Default_Headers.h"
#include "Utils/IM_Math.h"

class ImageProject
{


public:
	static void UI_FileMenueUI(class MasterEditor* CallingEditor);
	ImageProject(std::wstring ProjectNamem, IM_Math::Int2 ImageSize, class Renderer* renderer);
	ImageProject();
	~ImageProject();
		
	std::wstring GetProjectName() const;


	IM_Math::float2  GetCameraOffset() { return m_CameraOffset; }
	void SetCameraOffset(IM_Math::float2 NewCameraOffset) { m_CameraOffset = NewCameraOffset; UpdateCamera(); }

	CameraData GetCameraData() const { return m_CameraData; }
	
	class RenderTarget* GetOutputRT() const;
private:
	// Project description
	std::wstring m_ProjectName;
	IM_Math::Int2 m_ImageSize;

	//Camera
	IM_Math::float2 m_CameraOffset = IM_Math::float2(0,0);
	CameraData m_CameraData;
	void UpdateCamera();


	// Render Data
	std::unique_ptr<RenderTarget> m_OutputRT = nullptr;
	void Cleanup();
};