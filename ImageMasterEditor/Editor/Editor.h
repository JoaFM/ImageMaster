#pragma once
#include "MainApp/Default_Headers.h"
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>
#include "MainWindowUI.h"
#include "ImageProject.h"
#include "Brushes/BrushManager.h"

class MasterEditor
{
public:
	//System
	MasterEditor(std::wstring RootPath, HINSTANCE hInstance);
	void StartBlockingLoop();

	// tools
	void AddProject(std::string ProjectName, IM_Math::Int2 CanvasSize);
	class ImageProject* GetActiveProject() const { return m_ActiveProject; }

	Renderer* GetRenderer() { return m_Renderer.get(); }
	IM_Math::Int2 GetMouseCanvasPosition() { return m_MouseCanvasPosition; }
	std::wstring GetRootPath() const { return m_RootPath; };
	class Window* GetWindow() const;
	BrushManager* GetBrushManager();
	std::vector<std::unique_ptr<ImageProject>>& GetProjects() { return m_Projects; }

	void SetActiveProject(ImageProject* ProjectToSetAsActive);
private:
	//System
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<MainWindowUI> m_MainWindowUI;
	std::unique_ptr<BrushManager> m_BrushManager;

	//--
	double m_deltaTime = -1;
	std::wstring m_RootPath = L"";
	IM_Math::Int2 m_MouseCanvasPosition;
	bool DrawUI();

	// Workspace
	std::vector<std::unique_ptr<ImageProject>> m_Projects;
	ImageProject* m_ActiveProject = nullptr;
	INT32 ActiveProjectIndex = -1;
	void RefreshAssets();

	//--------- Behaviors
	void Behaviors(float DeltaTime);
	bool IsInModalState = false;
	bool m_Quiting = false;

	void UpdateState();


public: // --- Actions should go on the undo stack
	void Action_New();


};
