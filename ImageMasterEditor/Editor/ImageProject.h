#pragma once
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>
#include "MainApp/Default_Headers.h"
#include "Utils/IM_Math.h"
#include "Layer.h"

class ImageProject
{


public:
	static void UI_FileMenueUI(class MasterEditor* CallingEditor);
	ImageProject(std::string ProjectNamem, IM_Math::Int2 ImageSize, class Renderer* renderer);
	ImageProject();
	~ImageProject();
		
	std::string GetProjectName() const;


	IM_Math::float2  GetCameraOffset() { return m_CameraOffset; }
	void SetCameraOffset(IM_Math::float2 NewCameraOffset) { m_CameraOffset = NewCameraOffset; UpdateCamera(); }

	CameraData GetCameraData() const { return m_CameraData; }
	
	class RenderTarget* GetOutputRT() const;
	std::vector<std::unique_ptr<Layer>>& GetLayers();
	bool IsLayerSelected(const Layer* LayerInQuestion) const;
	void SetSelected(Layer* NewLayerToBeSelected);
private:
	// Project description
	std::string m_ProjectName;
	IM_Math::Int2 m_ImageSize;

	//Camera
	IM_Math::float2 m_CameraOffset = IM_Math::float2(0,0);
	CameraData m_CameraData;
	void UpdateCamera();


	// Render Data
	std::unique_ptr<RenderTarget> m_OutputRT = nullptr;
	void Cleanup();

	// Layers
	std::vector<std::unique_ptr<Layer>> m_Layers;
	Layer* m_SelectedLayer = nullptr;
	
};
