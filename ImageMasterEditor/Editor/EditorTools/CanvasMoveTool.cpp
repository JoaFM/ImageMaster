#include "MainApp/Default_Headers.h"
#include "CanvasMoveTool.h"
#include "../Editor.h"

CanvasMoveTool::CanvasMoveTool(class MasterEditor* Editor)
	: 
	EditorToolBase(Editor)
{

}

void CanvasMoveTool::UI_DrawToolbarIcon()
{
	bool IsActive = GetEditor()->IsToolActive(this) ;
	ImVec4 tint = IsActive ? ImVec4(1, 1, 1, 1) : ImVec4(.6f, .6f, .6f, .6f);  // tint_col

	if (Texture2D* Icon = GetEditor()->GetIcon(std::string("CanvasMove")))
	{
		if (ImGui::ImageButton((void*)Icon->GetSRV(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), tint))
		{
			TAUtils::Log("Go into move mode ");
			GetEditor()->AddSetActiveTool(this);
		}
	}
}

void CanvasMoveTool::UI_DrawToolSettings()
{
	ImGui::Text("I am a canvas move tool");
}

void CanvasMoveTool::Tick(float DeltaTime)
{
	//#TODO so there should be a list of tools/ behaviors that get activated and deactivated as needed. So every bahavoir is a tool
	ImageProject* m_ActiveProject = GetEditor()->GetActiveProject();
	// ----------------- Active project -----------
	if (m_ActiveProject == nullptr) { return; }

	// move m_ActiveProject Camera
	IM_Math::float2 CurrentOffset = m_ActiveProject->GetCameraOffset();
	{
		const float canvasSpeed = 1;
		CurrentOffset.x += GetWindow()->IsKeyDown(Window::KeyCode::A) ? canvasSpeed * DeltaTime : 0;
		CurrentOffset.x -= GetWindow()->IsKeyDown(Window::KeyCode::D) ? canvasSpeed * DeltaTime : 0;
		CurrentOffset.y += GetWindow()->IsKeyDown(Window::KeyCode::W) ? canvasSpeed * DeltaTime : 0;
		CurrentOffset.y -= GetWindow()->IsKeyDown(Window::KeyCode::S) ? canvasSpeed * DeltaTime : 0;
	}

	{
		if (!IsMovingMouse && GetWindow()->OnMouseDown(-1))
		{
			m_MouseMoveLoc = IM_Math::float2((float)GetWindow()->GetMouseX(), (float)GetWindow()->GetMouseY());
			IsMovingMouse = true;
		}
		else if (GetWindow()->OnMouseUp(-1))
		{
			IsMovingMouse = false;
		}
		IM_Math::float2 MouseOffset(0, 0);
		if (IsMovingMouse)
		{
			MouseOffset = m_MouseMoveLoc - IM_Math::float2((float)GetWindow()->GetMouseX(), (float)GetWindow()->GetMouseY());
			m_MouseMoveLoc = IM_Math::float2((float)GetWindow()->GetMouseX(), (float)GetWindow()->GetMouseY());
		}
		CurrentOffset += MouseOffset;
	}


	m_ActiveProject->SetCameraOffset(CurrentOffset);
}
