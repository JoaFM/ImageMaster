#include "pch.h"
#include "BrushTool.h"
#include "../Editor.h"

BrushTool::BrushTool(class MasterEditor* Editor)
	: 
	EditorToolBase(Editor)
{

}

void BrushTool::UI_DrawToolbarIcon()
{
	bool IsActive = GetEditor()->IsToolActive(this);
	ImVec4 tint = IsActive ? ImVec4(1, 1, 1, 1) : ImVec4(.6f, .6f, .6f, .6f);  // tint_col

	if (Texture2D* Icon = GetEditor()->GetIcon(std::string("Brush")))
	{
		if (ImGui::ImageButton((void*)Icon->GetSRV(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1),-1, ImVec4(0, 0,0,0), tint))
		{
			TAUtils::Log("Go into move mode ");
			GetEditor()->AddSetActiveTool(this);
		}
	}

}

void BrushTool::UI_DrawToolSettings()
{
	ImGui::Text("Spacing");
	ImGui::SliderFloat("##Spacing_S", &m_BrushSpacing, 0.1f, 2.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);

	ImGui::Text("Size");
	ImGui::SliderFloat("##Size_S", &m_BrushSize, 0, 100.0f, "%.1f");

	ImGui::Text("Alpha");
	ImGui::SliderFloat("##Alpha_S", &m_BrushAlpha, 0, 1.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);

	ImGui::Text("BrushFalloff");
	ImGui::SliderFloat("##BrushFalloff_S", &m_BrushFalloff, 0, 1.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);
}


void BrushTool::DrawBrushAtLocation(IM_Math::float2 BrushLocation)
{
	//Mouse
	RenderTypes::CB_BrushInput_Struct BrushInput;
	BrushInput.MouseButton = 1;
	BrushInput.MousePosition = BrushLocation;
	BrushInput.BrushSize = m_BrushSize;
	BrushInput.BrushAlpha = m_BrushAlpha;
	BrushInput.BrushFalloff = m_BrushFalloff;
	BrushInput.BrushMainColour = GetEditor()->GetForegroundColor();

	GetRenderer()->GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_BrushInput]->UpdateData(&BrushInput);

	if (Layer* PaintLayer = GetEditor()->GetActiveProject()->GetPaintLayer())
	{
		ComputeShader* BlendCP = GetRenderer()->GetComputeShaders()[L"Brush_Circle"].get();

		BlendCP->SetTexture("BufferOut", PaintLayer->GetCanvasTexture());
		BlendCP->SetTexture("CanvasTexture", nullptr);
		if (GetRenderer()->BindComputeShader(L"Brush_Circle"))
		{
			BlendCP->Dispatch();
			GetRenderer()->UnbindCurrentComputeShader();
		}

	}
}


void BrushTool::Tick(float DeltaTime)
{
	m_CurrentMouseLocation = GetEditor()->GetMouseCanvasPosition();;

	if (GetWindow()->OnMouseDown(0))
	{
		StartDarwing();
	}
	else if (GetWindow()->OnMouseUp(0))
	{
		EndDrawing();
	}
	else if (m_IsDrawing)
	{
		UpdateDrawing();
	}
}


UINT64 BrushTool::GetShortcut()
{
	return MasterEditor::KeyStateToUniqueKey(MasterEditor::BuildKeyModifierState(false, false, false), (UINT32)Window::KeyCode::B);
}

void BrushTool::UI_DrawOverlay(class ImageProject* CurrProject)
{
	if (m_IsDrawing) { return; }
	//Mouse
	RenderTypes::CB_BrushInput_Struct BrushInput;
	BrushInput.MouseButton = 1;
	BrushInput.MousePosition = m_CurrentMouseLocation;
	BrushInput.BrushSize = m_BrushSize;
	BrushInput.BrushAlpha = m_BrushAlpha;
	BrushInput.BrushFalloff = m_BrushFalloff;
	BrushInput.BrushMainColour = GetEditor()->GetForegroundColor();

	GetRenderer()->GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_BrushInput]->UpdateData(&BrushInput);

	if (Layer* PaintLayer = GetEditor()->GetActiveProject()->GetPaintLayer())
	{
		ComputeShader* BlendCP = GetRenderer()->GetComputeShaders()[L"Brush_Circle"].get();

		BlendCP->SetTexture("BufferOut", CurrProject->GetUI_RT());
		BlendCP->SetTexture("CanvasTexture", nullptr);
		if (GetRenderer()->BindComputeShader(L"Brush_Circle"))
		{
			BlendCP->Dispatch();
			GetRenderer()->UnbindCurrentComputeShader();
		}

	}
}

void BrushTool::StartDarwing()
{
	m_BrushLocation = m_LastMouseLocation = m_CurrentMouseLocation;
	m_BrushTraveledDistance = 0;
	m_IsDrawing = true;
}

void BrushTool::EndDrawing()
{
	m_IsDrawing = false;
	if (Layer* PaintLayer = GetEditor()->GetActiveProject()->GetPaintLayer())
	{
		if (Layer* SelectedLayer = GetEditor()->GetActiveProject()->GetSelectedLayer())
		{
			PaintLayer->Composite(SelectedLayer->GetCanvasTexture());
			PaintLayer->Clear();
		}
	}
}

void BrushTool::UpdateDrawing()
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