#include "MainApp/Default_Headers.h"
#include "MoveTool.h"
#include "../Editor.h"

MoveTool::MoveTool(class MasterEditor* Editor)
	: 
	EditorToolBase(Editor)
{

}

void MoveTool::UI_DrawToolbarIcon()
{
	bool IsActive = GetEditor()->GetActiveTool() == this;
	ImVec4 tint = IsActive ? ImVec4(1, 1, 1, 1) : ImVec4(.6f, .6f, .6f, .6f);  // tint_col

	if (Texture2D* Icon = GetEditor()->GetIcon(std::string("Move")))
	{
		if (ImGui::ImageButton((void*)Icon->GetSRV(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), tint))
		{
			TAUtils::Log("Go into move mode ");
			GetEditor()->SetActiveTool(this);
		}
	}
}

void MoveTool::UI_DrawToolSettings()
{
	ImGui::Text("I am a move tool");

}

void MoveTool::Tick(float deltaTime)
{
}