
#include "BrushManager.h"
#include "UXUI/Window.h"
#include "../Editor.h"

BrushManager::BrushManager(class MasterEditor* MainEditor)
{
	m_MainEditor = MainEditor;
	m_Renderer = MainEditor->GetRenderer();
	m_Window = MainEditor->GetWindow();
	m_Color = IM_Math::float3(1.0f, 1.0f, 1.0f);
}

void BrushManager::Tick(float DeltaTime)
{
	m_CurrentMouseLocation = m_MainEditor->GetMouseCanvasPosition();;

	if (m_Window->OnMouseDown(0))
	{
		StartDarwing();
	}
	else if (m_Window->OnMouseUp(0))
	{
		EndDrawing();
	}
	else if (m_IsDrawing)
	{
		UpdateDrawing();
	}
}



void BrushManager::StartDarwing()
{
	m_BrushLocation = m_LastMouseLocation = m_CurrentMouseLocation;
	m_BrushTraveledDistance = 0;
	m_IsDrawing = true;
}

void BrushManager::EndDrawing()
{
	m_IsDrawing = false;;
}

void BrushManager::UpdateDrawing()
{

	m_BrushSpacing = max(m_BrushSpacing, 0.1f);
	m_BrushSize = max(m_BrushSize, 0.1f);

	const float brushSpacingRadiusAjusted = m_BrushSize * (m_BrushSpacing * 2);
	float d = IM_Math::float2::Distance(m_CurrentMouseLocation, m_BrushLocation);
	
	while (d > brushSpacingRadiusAjusted)
	{

		IM_Math::float2 dir = m_CurrentMouseLocation - m_BrushLocation;
		dir.Normalize();
		m_BrushLocation += dir * brushSpacingRadiusAjusted;
		DrawBrushAtLocation(m_BrushLocation);
		d = IM_Math::float2::Distance(m_CurrentMouseLocation, m_BrushLocation);
	}

}

void BrushManager::DrawBrushAtLocation(IM_Math::float2 BrushLocation)
{
	//Mouse
	RenderTypes::CB_BrushInput_Struct BrushInput;
	BrushInput.MouseButton = 1;
	BrushInput.MousePosition = BrushLocation;
	BrushInput.BrushSize = m_BrushSize;
	BrushInput.BrushAlpha = m_BrushAlpha;
	BrushInput.BrushMainColour = m_Color;

	m_Renderer->GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_BrushInput]->UpdateData(&BrushInput);

	if (Layer* alayer = m_MainEditor->GetActiveProject()->GetSelectedLayer())
	{
		ComputeShader* BlendCP = m_Renderer->GetComputeShaders()[L"Brush_Circle"].get();

		BlendCP->SetTexture("BufferOut", alayer->GetCanvasTexture());
		BlendCP->SetTexture("CanvasTexture", nullptr);
		if (m_Renderer->BindComputeShader(L"Brush_Circle"))
		{
			BlendCP->Dispatch(m_Renderer->GetDeviceContext());
			m_Renderer->UnbindCurrentComputeShader();
		}

	}
}

void BrushManager::DrawBrushUI()
{
	ImGui::Text("Spacing");
	ImGui::SliderFloat("##Spacing_S", &m_BrushSpacing, 0.1f, 2.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);

	ImGui::Text("Size");
	ImGui::SliderFloat("##Size_S", &m_BrushSize, 0, 100.0f, "%.1f");

	ImGui::Text("Alpha");
	ImGui::SliderFloat("##Alpha_S", &m_BrushAlpha, 0, 1.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);
}

void BrushManager::DrawColorUI()
{
	float* Col = reinterpret_cast<float*>(&m_Color);
	ImGui::ColorEdit3("Foreground", Col, ImGuiColorEditFlags_NoInputs);
}
