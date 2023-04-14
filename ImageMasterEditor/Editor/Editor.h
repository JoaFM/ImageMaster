#pragma once
#include "MainApp/Default_Headers.h"
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>

class MasterEditor
{
public:
	void Start(HINSTANCE hInstance);
	void StartBlockingLoop();
private:
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Renderer> m_Renderer;

	double m_deltaTime = -1;

};
