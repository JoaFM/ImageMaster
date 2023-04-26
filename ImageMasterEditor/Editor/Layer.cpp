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
}

void Layer::Composite(RenderTarget* OutputRT)
{
	Renderer* renderer = m_ParentProject->GetRenderer();
	std::vector<std::string> LayerModes = m_ParentProject->GetLayerModesAsString();
	std::string CurrentModeKey = LayerModes[currentItem];

	if (renderer->BindComputeShader(TAUtils::CharToWString(CurrentModeKey.c_str())))
	{
		renderer->GetCurrentComputeShader()->SetRT(OutputRT);
		renderer->GetCurrentComputeShader()->Dispatch(renderer->GetDeviceContext());
	}
}

void Layer::UI_DrawLayer()
{

	std::vector<std::string> LayerModes = m_ParentProject->GetLayerModesAsString() ;
	
	char** Items = new char* [LayerModes.size()];
	const char eol = '\0';
	for (INT32 i = 0; i < LayerModes.size(); i++)
	{
		Items[i] = new char[LayerModes[i].size() +2];
		std::memcpy(Items[i]						, LayerModes[i].c_str()	, LayerModes[i].size()		);
		std::memcpy(&Items[i][LayerModes[i].size()]	, &eol					, 1							);
	}

	ImGui::Combo(("Mode##" + m_ID).c_str(), &currentItem, Items, (INT32)LayerModes.size());

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
		ImVec2(180, 30)))
	{
		m_ParentProject->SetSelected(this);
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	for (INT32 i = 0; i < LayerModes.size(); i++)
	{
		delete[] Items[i];
	}
	delete[] Items;

}

bool Layer::IsSelected() const
{
	return m_ParentProject->IsLayerSelected(this);
}
