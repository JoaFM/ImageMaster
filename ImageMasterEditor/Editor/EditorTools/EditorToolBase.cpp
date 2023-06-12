#include "EditorToolBase.h"
#include "../Editor.h"

EditorToolBase::EditorToolBase(class MasterEditor* Editor)
{
	m_MainEditor = Editor;
	m_Renderer = Editor->GetRenderer();
	m_Window = Editor->GetWindow();
}

void EditorToolBase::Setup()
{
	m_MainEditor->AddToolShortcut(this->GetShortcut(), this);
	m_MainEditor->AddToolsShortcutOnDown(this->GetShortcutWhilePressed(), this);
}

void EditorToolBase::UI_DrawToolbarIcon()
{

}

void EditorToolBase::UI_DrawToolSettings()
{

}
