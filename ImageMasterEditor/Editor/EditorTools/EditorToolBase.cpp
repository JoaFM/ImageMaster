#include "EditorToolBase.h"
#include "../Editor.h"

EditorToolBase::EditorToolBase(class MasterEditor* Editor)
{
	m_MainEditor = Editor;
	m_Renderer = Editor->GetRenderer();
	m_Window = Editor->GetWindow();

}

void EditorToolBase::UI_DrawToolbarIcon()
{

}

void EditorToolBase::UI_DrawToolSettings()
{

}
