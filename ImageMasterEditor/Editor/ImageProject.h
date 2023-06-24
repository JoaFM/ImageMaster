#pragma once
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>
#include "pch.h"
#include "Utils/IM_Math.h"
#include "Layer.h"

class ImageProject
{


public:
	ImageProject(std::string ProjectNamem, IM_Math::Int2 ImageSize, class Renderer* renderer, class MasterEditor* CallingEditor);
	ImageProject();
	~ImageProject();

	// Project
	static void UI_FileMenuNewUI(std::set<std::string>& Messages);
	void UI_FileMenuWindowUI();
	std::string GetProjectName() const;

	//Camera
	IM_Math::float2  GetCameraOffset() { return m_CameraOffset; }
	void SetCameraOffset(IM_Math::float2 NewCameraOffset) { m_CameraOffset = NewCameraOffset; UpdateCamera(); }
	CameraData GetCameraData() const { return m_CameraData; }
	float GetZoom();
	void SetZoom(float zoom);


	// Layer
	void AddLayer(std::string LayerName);
	IM_Math::Int2 GetSize() const { return m_ImageSize; }
	void DeleteLayer(Layer* LayerToDelete);
	Layer* FindActiveLayer();
	Layer* GetSelectedLayer();
	Layer* GetPaintLayer();
	bool IsLayerSelected(const Layer* LayerInQuestion) const;
	void SetSelected(Layer* NewLayerToBeSelected);
	std::vector<std::unique_ptr<Layer>>& GetLayers();
	std::vector<std::string> GetLayerModesAsString();


	//rendering
	void CompositeRender();
	class RenderTarget* GetOutputRT() const;
	void ReadBackOutput(const IM_Math::Int2& PixelPos, std::vector<float>& ReadbackBuffer8X8);

	//Ref
	class Renderer* GetRenderer() { return m_renderer; };
	class MasterEditor* GetEditor() { return m_Editor; };

private:
	// Project description
	std::string m_ProjectName;
	IM_Math::Int2 m_ImageSize;

	//Camera
	IM_Math::float2 m_CameraOffset = IM_Math::float2(0,0);
	CameraData m_CameraData;
	float m_zoom = 1;
	void UpdateCamera();

	// Render Data
	// Final display layer
	std::unique_ptr<RenderTarget> m_OutputRT = nullptr;
	void Cleanup();

	// references
	class Renderer* m_renderer = nullptr;
	class MasterEditor* m_Editor = nullptr;

	// Layers
	std::vector<std::unique_ptr<Layer>> m_Layers;
	std::unique_ptr<Layer> m_PaintLayer;
	Layer* m_SelectedLayer = nullptr;
	
};
