#include "pch.h"
#include "Layer.h"
#include <External/IMGUI/imgui.h>
#include "ImageProject.h"
#include "Utils/IM_STD.h"
#include <array>
#include "PopUps/SaveLayer.h"
#include "MainWindowUI.h"
#include "MainApp/Default_Defines.h"


Layer::Layer(std::string LayerName, class ImageProject* ParentProject)
{
	m_LayerName = LayerName;
	m_BlendMode = SHADER_BlendOver;
	m_ParentProject = ParentProject;
	m_ID = TAUtils::RandomString(10);
	m_CanvasTexture = std::make_unique<RenderTarget>("Layer_" + LayerName);
	m_RenderData.LayerSize = IM_Math::Int2(ParentProject->GetSize().x, ParentProject->GetSize().y);
	m_CanvasTexture->CreateTarget(ParentProject->GetSize().x, ParentProject->GetSize().y, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, ParentProject->GetRenderer());
}

void Layer::ReadBackData(char* Filepath)
{
	m_CanvasTexture->SaveLayerToPath(Filepath);
}


void Layer::Clear()
{
	m_CanvasTexture->Clear(0, 0, 0, 0, m_ParentProject->GetRenderer());
}

void Layer::Composite(RenderTarget* OutputRT)
{
	Renderer* renderer = m_ParentProject->GetRenderer();

	renderer->GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_Layer]->UpdateData(&m_RenderData);


	std::vector<std::string> LayerModes = m_ParentProject->GetLayerModesAsString();
	std::string CurrentModeKey = LayerModes[m_CurrentBlendMode];
	ComputeShader* BlendCP = m_ParentProject->GetRenderer()->GetComputeShaders()[TAUtils::CharToWString(("Blend_" + CurrentModeKey).c_str())].get();

	BlendCP->SetTexture("BufferOut", OutputRT);
	BlendCP->SetTexture("CanvasTexture", m_CanvasTexture.get());


	if (m_ParentProject->GetRenderer()->BindComputeShader(TAUtils::CharToWString(("Blend_" + CurrentModeKey).c_str())))
	{
		renderer->GetCurrentComputeShader()->Dispatch();
		BlendCP->Dispatch();
	}
}

void Layer::UI_DrawLayer(MainWindowUI* MainUI)
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

	if (ImGui::Button(m_LayerName.c_str(), ImVec2(160, 30)))
	{
		m_ParentProject->SetSelected(this);
	}
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Selectable("Delete Layer")) { m_ParentProject->DeleteLayer(this); }
		if (ImGui::Selectable("Save layer to Disk"))
		{
			MainUI->CurrentPopup = std::make_unique<Popup_SaveLayer>();
			reinterpret_cast<Popup_SaveLayer*>(MainUI->CurrentPopup.get())->LayerToSave = this;
		}
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
	return m_CurrentBlendMode;
}
