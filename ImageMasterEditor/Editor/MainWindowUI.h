#pragma once
#include "MainApp/Default_Headers.h"
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>

class MainWindowUI
{
public:
	MainWindowUI(class Window* ParentWindow, class MasterEditor* Editor);
	void DrawUI();

private:
	class Window* m_ParentWindow = nullptr;
	class MasterEditor* m_Editor = nullptr;


	void DrawAppUI();
	void SetGlobalStyle();
	void UI_DrawAppMenuBar();
};
