#include "pch.h"
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

	UINT64 t = this->GetShortcut();

	m_MainEditor->AddToolShortcut(t, this);
	

	UINT64 tt = this->GetShortcutWhilePressed();

	m_MainEditor->AddToolsShortcutOnDown(tt, this);
}

void EditorToolBase::UI_DrawToolbarIcon()
{

}

void EditorToolBase::UI_DrawToolSettings()
{

}
