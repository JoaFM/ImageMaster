#include "Layer.h"
#include <IMGUI/imgui.h>
#include "ImageProject.h"


Layer::Layer(std::string LayerName, class ImageProject* ParentProject)
{
	m_LayerName = LayerName;
	m_BlendMode = L"AlphaBlend";
	m_ParentProject = ParentProject;
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
		ImVec2(180, 50)))
	{
		m_ParentProject->SetSelected(this);
		OutputDebugStringA("Button1\n");
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

bool Layer::IsSelected() const
{
	return m_ParentProject->IsLayerSelected(this);
}
