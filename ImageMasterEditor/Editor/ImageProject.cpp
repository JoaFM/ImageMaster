
#include "pch.h"
#include "ImageProject.h"
#include <debugapi.h>
#include "Editor.h"
#include "d3d9.h"
#include <string>
#include "Engine/Renderer/RenderUtils.h"

ImageProject::ImageProject(std::string ProjectName, IM_Math::Int2 ImageSize, class Renderer* renderer, class MasterEditor* CallingEditor)
{
	m_ProjectName = ProjectName;
	m_ImageSize = ImageSize;
	m_renderer = renderer;
	m_Editor = CallingEditor;

	m_OutputRT = std::make_unique<RenderTarget>("ImageProject_MainRT_" + ProjectName);;
	m_OutputRT->CreateTarget(m_ImageSize.x, m_ImageSize.y, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, m_renderer);
	
	m_UI_Target = std::make_unique<RenderTarget>("ImageProject_UI_Layer" + ProjectName );
	m_UI_Target->CreateTarget(m_ImageSize.x, m_ImageSize.y, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, m_renderer);

	m_CameraOffset = IM_Math::float2(-300, -90);
	AddLayer("Layer 1");
	AddLayer("Layer 2");
	m_PaintLayer = std::make_unique<Layer>("$PaintLayer", this);

	SetSelected(m_Layers[m_Layers.size()-1].get());
}

ImageProject::ImageProject()
{
	Cleanup();
}

ImageProject::~ImageProject()
{
	Cleanup();
}

std::string ImageProject::GetProjectName() const
{
	return m_ProjectName;
}

class RenderTarget* ImageProject::GetOutputRT() const
{
	return m_OutputRT.get();
}

std::vector<std::unique_ptr<Layer>>& ImageProject::GetLayers()
{
	return m_Layers;
}

bool ImageProject::IsLayerSelected(const Layer* LayerInQuestion) const
{
	return m_SelectedLayer == LayerInQuestion;
}

void ImageProject::SetSelected(Layer* NewLayerToBeSelected)
{
	m_SelectedLayer = NewLayerToBeSelected;
}

void ImageProject::CompositeRender()
{
	RenderUtils::ScopedProfile Scope(GetRenderer(), std::wstring(L"CompositeRender"));

	m_OutputRT->Clear(0, 0, 0, 1, m_renderer);
	for (auto& Layer : m_Layers)
	{
		if (Layer->IsSelected())
		{
			RenderUtils::ScopedProfile Scope(GetRenderer(), std::wstring(L"Inject Paint Layer"));
			m_PaintLayer->Composite(m_OutputRT.get());
		}
		Layer->Composite(m_OutputRT.get());
	}

}

void ImageProject::DeleteLayer(Layer* LayerToDelete)
{
 	std::vector<std::unique_ptr<Layer>>::iterator object =
 		find_if(m_Layers.begin(), m_Layers.end(),
 			[&](std::unique_ptr<Layer>& obj) { return obj.get() == LayerToDelete; }
 	);
 	m_Layers.erase(object);
}

Layer* ImageProject::GetSelectedLayer()
{
	return m_SelectedLayer;
}

Layer* ImageProject::GetPaintLayer()
{
	return m_PaintLayer.get();
}

float ImageProject::GetZoom()
{
	return m_zoom;
}

void ImageProject::SetZoom(float zoom)
{
	m_zoom = zoom;
}

void ImageProject::AddLayer(std::string LayerName)
{
	m_Layers.push_back(std::make_unique<Layer>(LayerName, this));
}

void ImageProject::UpdateCamera()
{
	m_CameraData.Transform.Position.x = m_CameraOffset.x;
	m_CameraData.Transform.Position.y = m_CameraOffset.y;
}

void ImageProject::Cleanup()
{
	m_ProjectName = std::string("");
	m_ImageSize = IM_Math::Int2(-1, -1);
	m_OutputRT = nullptr;
}

std::vector<std::string> ImageProject::GetLayerModesAsString()
{
	std::map<std::wstring, std::unique_ptr<ComputeShader>>& shaders = m_renderer->GetComputeShaders();

	std::vector<std::string> result;

	for (auto& shader : shaders)
	{
		if (shader.first.starts_with(L"Blend_"))
		{
			result.push_back(TAUtils::WStringToChar(shader.first.substr(6, shader.first.size()-6).c_str()) + "\0");
		}
	}

	return result;
}

Layer* ImageProject::FindActiveLayer()
{
	for (INT32 i = 0; i < m_Layers.size(); i++)
	{
		if (m_Layers[i]->IsSelected())
		{
			return m_Layers[i].get();
		}
	}
	return nullptr;

}

void ImageProject::ReadBackOutput(const IM_Math::Int2& PixelPos, std::vector<float>& ReadbackBuffer8X8)
{
	RenderTarget* RT = GetOutputRT();
	RT->CopyBackPixel(PixelPos, ReadbackBuffer8X8);
}

void ImageProject::UI_FileMenuNewUI( std::set<std::string>& Messages)
{
	if (ImGui::MenuItem("New"))
	{
		Messages.insert("NewProjectPopUp");
	}
}

void ImageProject::UI_FileMenuWindowUI()
{
	if (ImGui::MenuItem((this->GetProjectName() + "##___WindowMenu").c_str()))
	{
		m_Editor->SetActiveProject(this);
	}
}
