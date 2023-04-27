#include "Layer.h"
#include <IMGUI/imgui.h>
#include "ImageProject.h"
#include "Utils/IM_STD.h"
#include <array>


Layer::Layer(std::string LayerName, class ImageProject* ParentProject)
{
	m_LayerName = LayerName;
	m_BlendMode = L"AlphaBlend";
	m_ParentProject = ParentProject;
	m_ID = TAUtils::RandomString(10);
	m_CanvasTexture = std::make_unique<RenderTarget>();
	m_CanvasTexture->CreateTarget(512, 512, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, ParentProject->GetRenderer());

}

void Layer::Composite(RenderTarget* OutputRT)
{
	Renderer* renderer = m_ParentProject->GetRenderer();
	std::vector<std::string> LayerModes = m_ParentProject->GetLayerModesAsString();
	std::string CurrentModeKey = LayerModes[currentItem];

	if (renderer->BindComputeShader(TAUtils::CharToWString(CurrentModeKey.c_str())))
	{
		renderer->GetCurrentComputeShader()->SetTexture("BufferOut", OutputRT);
		renderer->GetCurrentComputeShader()->SetTexture("CanvasTexture", m_CanvasTexture.get());
		renderer->GetCurrentComputeShader()->Dispatch(renderer->GetDeviceContext());
	}
}

void Layer::UI_DrawLayer()
{
	
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(.1f, 0.5f));

	if (IsSelected())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f });
	}

	if (ImGui::Button(
		m_LayerName.c_str(),
		ImVec2(160, 30)))
	{
		m_ParentProject->SetSelected(this);
	}
	if (ImGui::BeginPopupContextItem())
	{
		ImGui::Text("Layer");
		ImGui::Text("-------");
		if (ImGui::Selectable("Delete Layer")) { m_ParentProject->DeleteLayer(this); }
		ImGui::EndPopup();
	}
	
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

bool Layer::IsSelected() const
{
	return m_ParentProject->IsLayerSelected(this);
}

int& Layer::GetCurrentItemIndex()
{
	return currentItem;
}
