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
	void UI_DrawToolSelectorBar();
	void UI_DrawColourUI();

	std::vector<IM_Math::float3> m_Swatches = {
	{0,0,0},{1,1,1},{1,0,0},{0,1,0},{0,0,1},
	{1,1,0},{0,1,1},{1,0,1},{.025f,.025f,.025f},{.05f,.05f,.05f},
	{.1f,.1f,.1f},{.2f,.2f,.2f},{.3f,.3f,.3f},{.4f,.4f,.4f},{.5f,.5f,.5f},
	{.6f,.6f,.6f},{.7f,.7f,.7f},{.8f,.8f,.8f},{.9f,.9f,.9f},

	};

};
