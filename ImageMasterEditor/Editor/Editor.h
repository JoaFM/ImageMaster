#pragma once
#include "MainApp/Default_Headers.h"
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>
#include "MainWindowUI.h"
#include "ImageProject.h"

class MasterEditor
{
public:
	

	void Start(HINSTANCE hInstance);
	void StartBlockingLoop();

	void AddProject(std::wstring ProjectName);
private:
	//System
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<MainWindowUI> m_MainWindowUI;
	double m_deltaTime = -1;
	void DrawUI();

	// Workspace
	std::vector<std::unique_ptr<ImageProject>> m_Projects;
	ImageProject* m_ActiveProject = nullptr;
	INT32 ActiveProjectIndex = -1;

	//--------- Behaviors
	void Behaviors();
	bool m_Quiting = false;

};
