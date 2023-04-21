#include "ImageProject.h"
#include <debugapi.h>
#include "Editor.h"
#include <string>

ImageProject::ImageProject(std::wstring ProjectName, IM_Math::Int2 ImageSize, class Renderer* renderer)
{
	m_ProjectName = ProjectName;
	m_ImageSize = ImageSize;
	m_OutputRT = std::make_unique<RenderTarget>();;
	m_OutputRT->CreateTarget(m_ImageSize.x, m_ImageSize.y, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, renderer);
}

ImageProject::ImageProject()
{
	Cleanup();
}

ImageProject::~ImageProject()
{
	Cleanup();
}

std::wstring ImageProject::GetProjectName() const
{
	return m_ProjectName;
}

class RenderTarget* ImageProject::GetOutputRT() const
{
	return m_OutputRT.get();
}

void ImageProject::UpdateCamera()
{
	m_CameraData.Transform.Position.x = m_CameraOffset.x;
	m_CameraData.Transform.Position.y = m_CameraOffset.y;
}

void ImageProject::Cleanup()
{
	m_ProjectName = std::wstring(L"");
	m_ImageSize = IM_Math::Int2(-1, -1);
	m_OutputRT = nullptr;
}

void ImageProject::UI_FileMenueUI(class MasterEditor* CallingEditor)
{
	if (ImGui::MenuItem("New"))
	{
		OutputDebugStringA("\n File->New Project");
		CallingEditor->AddProject(std::wstring(L"New Project"));
	}
}
