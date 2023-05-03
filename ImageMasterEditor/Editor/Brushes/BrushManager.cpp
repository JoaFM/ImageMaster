
#include "BrushManager.h"
#include "UXUI/Window.h"
#include "../Editor.h"

BrushManager::BrushManager(class MasterEditor* MainEditor)
{
	m_MainEditor = MainEditor;
	m_Renderer = MainEditor->GetRenderer();
	m_Window = MainEditor->GetWindow();
}

void BrushManager::Tick(float DeltaTime)
{
	IM_Math::Int2 CanvasMousePos = m_MainEditor->GetMouseCanvasPosition();
	m_ThisLocation = IM_Math::float2((float)CanvasMousePos.x, (float)CanvasMousePos.y);


	if (m_Window->OnMouseDown(0))
	{
		m_BrushTraveledDistance = 0;
		m_IsDrawing = true;
		StartDarwing();
	}
	else if (m_Window->OnMouseUP(0))
	{
		EndDrawing();
		m_IsDrawing = false;;
	}
	else if (m_IsDrawing)
	{
		UpdateDrawing();
	}


}



void BrushManager::StartDarwing()
{
	DrawBrushAtCurrentLocation();
}

void BrushManager::EndDrawing()
{
	DrawBrushAtCurrentLocation();
}

void BrushManager::UpdateDrawing()
{
	float d = IM_Math::float2::Distance(m_ThisLocation, m_LastLocation);
	m_BrushTraveledDistance += d;
	if (m_BrushTraveledDistance > m_BrushSpacing)
	{
		DrawBrushAtCurrentLocation();
		m_LastLocation = m_ThisLocation;
		m_BrushTraveledDistance = 0;
	}
}

void BrushManager::DrawBrushAtCurrentLocation()
{
	//Mouse
	RenderTypes::CB_BrushInput_Struct BrushInput;
	BrushInput.MouseButton = 1;
	BrushInput.MousePosition = m_ThisLocation;
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
	ImGui::SliderFloat("##Spacing_S", &m_BrushSpacing, 0, 200, "%.1f");

	ImGui::Text("Size");
	ImGui::SliderFloat("##Size_S", &m_BrushSize, 0, 100, "%.01f");

	ImGui::Text("Alpha");
	ImGui::SliderFloat("##Alpha_S", &m_BrushAlpha, 0, 1, "%.01f");
}

void BrushManager::DrawColorUI()
{
	float* Col = reinterpret_cast<float*>(&m_Color);
	ImGui::ColorEdit3("Foreground", Col);
}
