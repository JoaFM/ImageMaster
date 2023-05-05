#pragma once
#include "MainApp/Default_Headers.h"
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>
#include "PopUps/PopUp.h"

class MainWindowUI
{
public:
	MainWindowUI(class Window* ParentWindow, class MasterEditor* Editor);
	bool DrawUI();
public:
	std::unique_ptr<PopupBase> CurrentPopup;

private:
	class Window* m_ParentWindow = nullptr;
	class MasterEditor* m_Editor = nullptr;

	void UI_DrawLayer();

	void UI_SetGlobalStyle();
	void UI_DrawAppMenuBar(std::set<std::string>& Messages);

	ImFont* m_mainFont = nullptr;
	void UI_DrawDebug();
	void UI_DrawBrushUI();
	bool UI_DrawPopUps(std::set<std::string>& Messages);
};
