#pragma once
#include "MainApp/Default_Headers.h"
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>

class MainWindowUI
{
public:
	MainWindowUI(class Window* ParentWindow);
	void DrawUI(class ImageProject* ActiveProject, class MasterEditor* Editor);

private:
	class Window* m_ParentWindow = nullptr;

	void DrawAppUI(class ImageProject* ActiveProject, class MasterEditor* Editor);
};
