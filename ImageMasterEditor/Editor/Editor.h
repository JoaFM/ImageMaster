#pragma once
#include "MainApp/Default_Headers.h"
#include <UXUI/Window.h>
#include <Engine/Renderer/Renderer.h>
#include "MainWindowUI.h"
#include "ImageProject.h"
#include "Engine/Renderer/Texture2D.h"
#include "EditorTools/EditorToolBase.h"

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
	IM_Math::float2 GetMouseCanvasPosition();
	std::wstring GetRootPath() const { return m_RootPath; };
	class Window* GetWindow() const;
	std::vector<std::unique_ptr<ImageProject>>& GetProjects() { return m_Projects; }

	void SetActiveProject(ImageProject* ProjectToSetAsActive);
	class Texture2D* GetIcon(std::string IconName);
	void AddSetActiveTool(EditorToolBase* NewTool);
	std::vector<class EditorToolBase*>& GetActiveTool() { return m_ActiveTools; }
	
	const std::vector<std::unique_ptr<EditorToolBase>>& GetTools();
	IM_Math::float3& GetForegroundColor() { return ForegroundColor; }
	void SetForegroundColour(IM_Math::float4 NewColor);

	bool IsToolActive(EditorToolBase* ToolToCheck) const;
	void AddToolShortcut(UINT64 Key, EditorToolBase* Tool);
	void AddToolsShortcutOnDown(UINT64 Key, EditorToolBase* Tool);

	void ClearActiveLayer(int NewClearState);

public://util
	static UINT64 KeyStateToUniqueKey(UINT32 ModifierState, UINT32 Key);
	static UINT32 BuildKeyModifierState(bool shift, bool ctrl, bool alt) ;
	void RefreshAssets();
	IM_Math::float4 GetUnderMousePixel() const { return m_UnderMousePixelValue; }

private:
	//System
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<MainWindowUI> m_MainWindowUI;

	//--
	double m_deltaTime = -1;
	std::wstring m_RootPath = L"";
	IM_Math::float2 m_MouseCanvasPosition;
	IM_Math::float2 m_MouseMoveLoc;
	bool IsMovingMouse = false;
	IM_Math::float3 ForegroundColor = IM_Math::float3(1,1,1);

	// Workspace
	std::vector<std::unique_ptr<ImageProject>> m_Projects;
	ImageProject* m_ActiveProject = nullptr;
	INT32 ActiveProjectIndex = -1;
	IM_Math::float4 m_UnderMousePixelValue;


	//--------- Behaviors
	void Behaviors(float DeltaTime);
	bool IsInModalState = false;
	bool m_Quiting = false;

	void UpdateState();
	void UpdateUnderMousePixel();

	void RefreshIcons(std::vector<std::string> FoundIcons);
	std::map<std::string, std::unique_ptr<Texture2D>> m_Icons;

	void LoadTools();
	std::vector<std::unique_ptr<EditorToolBase>> m_EditorTools;
	std::map<UINT64, EditorToolBase*> m_ToolsShortcut;
	std::map<UINT64, EditorToolBase*> m_ToolsShortcutOnDown;

	std::vector<class EditorToolBase*> m_ActiveTools;
	class EditorToolBase* m_OverrideUniqueTool = nullptr;

	bool DrawUI();
public:
};
