#pragma once
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>
#include "MainApp/Default_Headers.h"
#include "Utils/IM_Math.h"
#include "Layer.h"

class ImageProject
{


public:
	static void UI_FileMenuNewUI(std::set<std::string>& Messages);
	void UI_FileMenuWindowUI();
	ImageProject(std::string ProjectNamem, IM_Math::Int2 ImageSize, class Renderer* renderer, class MasterEditor* CallingEditor);
	ImageProject();
	~ImageProject();
		
	std::string GetProjectName() const;


	IM_Math::float2  GetCameraOffset() { return m_CameraOffset; }
	void SetCameraOffset(IM_Math::float2 NewCameraOffset) { m_CameraOffset = NewCameraOffset; UpdateCamera(); }
	//void SetCameraZoom(float zoom) { m_CameraData.Transform.Scale = ; UpdateCamera(); }

	CameraData GetCameraData() const { return m_CameraData; }
	
	class RenderTarget* GetOutputRT() const;
	std::vector<std::unique_ptr<Layer>>& GetLayers();
	bool IsLayerSelected(const Layer* LayerInQuestion) const;
	void SetSelected(Layer* NewLayerToBeSelected);
	IM_Math::Int2 GetSize() const { return m_ImageSize; }

	void CompositeRender();
	void DeleteLayer(Layer* LayerToDelete);


	Layer* GetSelectedLayer();

	float GetZoom();
	void SetZoom(float zoom);
private:
	// Project description
	std::string m_ProjectName;
	IM_Math::Int2 m_ImageSize;
	class MasterEditor* m_Editor = nullptr;
	//Camera
	IM_Math::float2 m_CameraOffset = IM_Math::float2(0,0);
	CameraData m_CameraData;
	float m_zoom = 1;
	void UpdateCamera();

	// Render Data
	std::unique_ptr<RenderTarget> m_OutputRT = nullptr;
	void Cleanup();
	class Renderer* m_renderer = nullptr;
	// Layers
	std::vector<std::unique_ptr<Layer>> m_Layers;
	Layer* m_SelectedLayer = nullptr;
	
public:
	std::vector<std::string> GetLayerModesAsString();
	class Renderer* GetRenderer() {return m_renderer; };
	Layer* FindActiveLayer();
	class MasterEditor* GetEditor() { return m_Editor; };
};
