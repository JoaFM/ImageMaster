#include "pch.h"
#include "EditorShortcuts.h"
#include "../Editor.h"

EditorShortcuts::EditorShortcuts(class MasterEditor* Editor)
	: 
	EditorToolBase(Editor)
{
	SetToolType(EditorToolBase::ToolType::ToolType_NoneUnique);
}

void EditorShortcuts::UI_DrawToolbarIcon()
{
}

void EditorShortcuts::UI_DrawToolSettings()
{
}

void EditorShortcuts::Tick(float DeltaTime)
{
	if (GetWindow()->IsKeyDown(Window::KeyCode::F5))
	{
		GetEditor()->RefreshAssets();
	}
	if (GetWindow()->IsKeyDown(Window::KeyCode::Backspace) && GetWindow()->IsKeyDown(Window::KeyCode::Control))
	{
		GetEditor()->ClearActiveLayer(-1);
	}
	if (GetWindow()->IsKeyDown(Window::KeyCode::Backspace) && GetWindow()->IsKeyDown(Window::KeyCode::Shift))
	{
		GetEditor()->ClearActiveLayer(1);
	}
	

	if (GetWindow()->IsKeyDown(Window::KeyCode::Control) && GetWindow()->OnMouseDown(0))
	{
		GetEditor()->SetForegroundColour(GetEditor()->GetUnderMousePixel());
	}
}


UINT64 EditorShortcuts::GetShortcut()
{
	return 0;
}


