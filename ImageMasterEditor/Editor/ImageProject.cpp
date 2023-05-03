#include "ImageProject.h"
#include <debugapi.h>
#include "Editor.h"
#include <string>

ImageProject::ImageProject(std::string ProjectName, IM_Math::Int2 ImageSize, class Renderer* renderer)
{
	m_ProjectName = ProjectName;
	m_ImageSize = ImageSize;
	m_renderer = renderer;
	m_OutputRT = std::make_unique<RenderTarget>("ImageProject_MainRT_" + ProjectName);;
	m_OutputRT->CreateTarget(m_ImageSize.x, m_ImageSize.y, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, m_renderer);
	m_CameraOffset = IM_Math::float2(-300, -90);
	m_Layers.push_back(std::make_unique<Layer>("Layer 1", this));
	m_Layers.push_back(std::make_unique<Layer>("Layer 2", this));
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
	m_OutputRT->Clear(0, 0, 0, 1, m_renderer);
	for (auto& Layer : m_Layers)
	{
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

void ImageProject::UI_FileMenueUI(class MasterEditor* CallingEditor)
{
	if (ImGui::MenuItem("New"))
	{
		OutputDebugStringA("\n File->New Project");
		CallingEditor->AddProject(std::string("New Project"));
	}
}
