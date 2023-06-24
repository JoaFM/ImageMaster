#include "pch.h"

#include "Editor.h"

#include "EditorTools/MoveTool.h"
#include "EditorTools/BrushTool.h"
#include "EditorTools/CanvasMoveTool.h"
#include "EditorTools/EditorShortcuts.h"
#include "Engine/Renderer/RenderUtils.h"

MasterEditor::MasterEditor(std::wstring RootPath, HINSTANCE hInstance)
{
	m_RootPath = RootPath;
	m_Window = std::make_unique<Window>(hInstance);
	m_Window->Init(1280, 720);
	m_Window->ShowTheWindow();

	m_Renderer = std::make_unique<Renderer>();
	m_Renderer->Init(IM_Math::Int2(1280, 720), m_Window.get());

	m_MainWindowUI = std::make_unique<MainWindowUI>(m_Window.get(),this);

	RefreshAssets();

	LoadTools();


#ifdef _DEBUG
	// Just add a default project just so we don't start on an empty page
	AddProject(std::string("Editor test"), IM_Math::Int2(512,512));
#endif // DEBUG

}



void MasterEditor::StartBlockingLoop()
{
	OutputDebugStringA("\n !!! Start App Loop !!! ");
	auto t_start = std::chrono::high_resolution_clock::now();
	auto t_end = std::chrono::high_resolution_clock::now();

	while (!m_Quiting && m_Window && m_Window->DoneAndWaitingToQuit())
	{
		// Delta Time
		t_end = std::chrono::high_resolution_clock::now();
		m_deltaTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		t_start = std::chrono::high_resolution_clock::now();
		 
		//Prep
		m_Renderer->ReadyNextFrame(m_Window.get());

		if (m_ActiveProject)
		{
			RenderUtils::ScopedProfile Scope(GetRenderer(), std::wstring(L"Active Project"));

			UpdateState();

			m_ActiveProject->CompositeRender();

			m_Renderer->UpdateCamera(m_ActiveProject->GetCameraData());
			m_Renderer->SetOutputRT(m_ActiveProject->GetOutputRT());
			m_Renderer->SetUIRT(m_ActiveProject->GetUI_RT());
			

			if (!IsInModalState)
			{
				ProcessTools();
			}

			GetRenderer()->m_PerfObject->EndEvent();

			UpdateUnderMousePixel();
			
			DrawViewport();
		
		}

		// UI
		IsInModalState = DrawUI();
		
		// Calc and present
		m_Renderer->Present(m_Window.get());

		// Windows message loop
		m_Window->PumpWindowMessages();

		// Input
		if (!IsInModalState)
		{
			Behaviors((float)m_deltaTime);
		}
	}
}

void MasterEditor::AddProject(std::string ProjectName, IM_Math::Int2 CanvasSize)
{
	m_Projects.push_back(std::make_unique<ImageProject>(ProjectName, CanvasSize, m_Renderer.get(), this));
	ActiveProjectIndex = (INT32)(m_Projects.size() - 1);
	SetActiveProject(m_Projects[ActiveProjectIndex].get());
}

IM_Math::float2 MasterEditor::GetMouseCanvasPosition()
{
	if (m_ActiveProject) 
	{
		return m_MouseCanvasPosition / m_ActiveProject->GetZoom();
	}
	
	return IM_Math::float2(-1,-1);
}

class Window* MasterEditor::GetWindow() const
{
	return m_Window.get();
}

void MasterEditor::SetActiveProject(ImageProject* ProjectToSetAsActive)
{
	m_ActiveProject = ProjectToSetAsActive; 
	if (m_ActiveProject) { m_Window->SetTitle("Image Master - " + m_ActiveProject->GetProjectName()); }
	else { m_Window->SetTitle("Image Master"); }
}

bool MasterEditor::DrawUI()
{
	RenderUtils::ScopedProfile Scope(GetRenderer(), std::wstring(L"DrawUI"));
	return m_MainWindowUI->DrawUI();
}

void MasterEditor::DrawViewport()
{
	RenderUtils::ScopedProfile Scope(GetRenderer(), std::wstring(L"DrawViewport"));
	m_Renderer->DrawViewMesh(m_Window.get());
	m_Renderer->SetRenderSize(m_ActiveProject->GetSize() * m_ActiveProject->GetZoom());
}

void MasterEditor::ProcessTools()
{
	RenderUtils::ScopedProfile Scope(GetRenderer(), std::wstring(L"Tools"));
	GetActiveProject()->GetUI_RT()->Clear(0, 0, 0, 0, GetRenderer());

	for (EditorToolBase* ActiveTool : m_ActiveTools)
	{
		if (!(ActiveTool->GetToolType() == EditorToolBase::ToolType::ToolType_UniqueSubmissive && m_OverrideUniqueTool))
		{
			ActiveTool->Tick((float)m_deltaTime);
			ActiveTool->UI_DrawOverlay(GetActiveProject());
		}
	}
	if (m_OverrideUniqueTool)
	{
		m_OverrideUniqueTool->Tick((float)m_deltaTime);
	}
}

void MasterEditor::SetForegroundColour(IM_Math::float4 NewColor)
{
	ForegroundColor = IM_Math::float3(NewColor.x, NewColor.y, NewColor.z);
}

void MasterEditor::UpdateUnderMousePixel()
{
	RenderUtils::ScopedProfile Scope(GetRenderer(), std::wstring(L"UpdateUnderMousePixel"));

	std::vector<float> ReadbackBuffer8X8;
	ReadbackBuffer8X8.assign(8 * 8 * 4, 0);
	IM_Math::float2 MousePos = GetMouseCanvasPosition();
	m_ActiveProject->ReadBackOutput(IM_Math::Int2((INT32)MousePos.x, (INT32)MousePos.y), ReadbackBuffer8X8);
	m_UnderMousePixelValue.x = ReadbackBuffer8X8[0];
	m_UnderMousePixelValue.y = ReadbackBuffer8X8[1];
	m_UnderMousePixelValue.z = ReadbackBuffer8X8[2];
	m_UnderMousePixelValue.w = ReadbackBuffer8X8[3];

}

UINT32 MasterEditor::BuildKeyModifierState(bool shift, bool ctrl, bool alt)  
{
	UINT32 ModState =
		shift ? 1 : 0 +
		ctrl ? 2 : 0 +
		alt ? 4 : 0;
	return ModState;
}

void MasterEditor::Behaviors(float DeltaTime)
{
	
	Window::KeyCode ActiveKey = m_Window->GetActiveKey();
	if (ActiveKey != (Window::KeyCode)0)
	{
		UINT32 ModState = BuildKeyModifierState(
			m_Window->IsKeyDown(Window::KeyCode::Shift),
			m_Window->IsKeyDown(Window::KeyCode::Control),
			m_Window->IsKeyDown(Window::KeyCode::Alt)
		);


		UINT64 ActiveKeyCode = KeyStateToUniqueKey(ModState, (UINT32)ActiveKey);

		if (m_ToolsShortcut.find(ActiveKeyCode) != m_ToolsShortcut.end())
		{
			AddSetActiveTool(m_ToolsShortcut[ActiveKeyCode]);
		}


		if (m_ToolsShortcutOnDown.find(ActiveKeyCode) != m_ToolsShortcutOnDown.end())
		{
			if (m_OverrideUniqueTool == nullptr || (m_OverrideUniqueTool && m_ToolsShortcutOnDown[ActiveKeyCode] != m_OverrideUniqueTool))
			{
				m_OverrideUniqueTool = m_ToolsShortcutOnDown[ActiveKeyCode];
			}
		}
		else
		{
			m_OverrideUniqueTool = nullptr;
		}
	}
	else
	{
		m_OverrideUniqueTool = nullptr;
	}
	// quit on escape
	m_Quiting = m_Window->OnKeyDown(Window::KeyCode::Escape);

	// Cycle open projects 
	if (m_Window->OnKeyDown(Window::KeyCode::End))
	{
		if (m_Projects.size() >= 0)
		{
			ActiveProjectIndex++;
			if (ActiveProjectIndex >= m_Projects.size())
			{
				ActiveProjectIndex = 0;
			}
			SetActiveProject(m_Projects[ActiveProjectIndex].get());
		}
		else 
		{
			SetActiveProject(nullptr);
			ActiveProjectIndex = -1;
		}
	}


 	IM_Math::float2 CurrentOffset = m_ActiveProject->GetCameraOffset();

 	const float offset = (float)m_Window->MouseScroll();
 	const float ZoomStep = 2.0f;
 	if (offset > 0)
 	{
 		m_ActiveProject->SetZoom(m_ActiveProject->GetZoom() / ZoomStep);
 		CurrentOffset -= GetMouseCanvasPosition() * (m_ActiveProject->GetZoom() *.5f);
 	}
 	else if (offset < 0)
 	{
 		m_ActiveProject->SetZoom(m_ActiveProject->GetZoom() * ZoomStep);
 		CurrentOffset += GetMouseCanvasPosition() * m_ActiveProject->GetZoom();
 	}
 
 	m_ActiveProject->SetCameraOffset(CurrentOffset);

	if (m_Window->IsKeyDown(Window::KeyCode::F))
	{
		m_ActiveProject->SetCameraOffset(IM_Math::float2(-100,-100));
	}
}

void MasterEditor::UpdateState()
{
	RenderUtils::ScopedProfile Scope(GetRenderer(), std::wstring(L"UpdateState"));
	IM_Math::float2 CamPos = GetActiveProject()->GetCameraOffset();
	m_MouseCanvasPosition = IM_Math::float2((float)(CamPos.x + m_Window->GetMouseX()), (float)(CamPos.y + m_Window->GetMouseY()));
}


void MasterEditor::RefreshAssets()
{
	OutputDebugStringW(L"\n----- Finding Assets ---- ");
	OutputDebugStringW(L"\n Shaders");

	std::wstring ContentFolder = m_RootPath + L"\\Content";
	std::wstring ShaderFolder = ContentFolder + L"\\Shaders";
	std::wstring ComputeShaderFolder = ContentFolder + L"\\ComputeShaders";
	std::wstring IconFolder = ContentFolder + L"\\Icons";

	std::vector<std::wstring> FoundShaders;
	for (const auto& entry : std::filesystem::directory_iterator(ShaderFolder))
	{
		FoundShaders.push_back(entry.path().c_str());

		std::wstring FoundShader(L"\n\t Shader :\t");
		FoundShader += entry.path().c_str();
		OutputDebugStringW(FoundShader.c_str());
	}
	OutputDebugStringW(L"\n ComputeShaders");

	std::vector<std::wstring> FoundComputeShaders;
	for (const auto& entry : std::filesystem::directory_iterator(ComputeShaderFolder))
	{
		FoundComputeShaders.push_back(entry.path().c_str());

		std::wstring FoundShader(L"\n\t ComputeShader :\t");
		FoundShader += entry.path().c_str();
		OutputDebugStringW(FoundShader.c_str());
	}
	OutputDebugStringW(L"\n Icons");

	std::vector<std::string> FoundIcons;
	for (const auto& entry : std::filesystem::directory_iterator(IconFolder))
	{
		FoundIcons.push_back(TAUtils::WStringToChar(entry.path().c_str()));

		std::wstring FoundIcon(L"\n\t Icon: :\t");
		FoundIcon += entry.path().c_str();
		OutputDebugStringW(FoundIcon.c_str());
	}

	OutputDebugStringW(L"\n-------------------------------\nLoading");
	OutputDebugStringW(L"\n\tShaders");
	m_Renderer->RefreshShaders(FoundShaders, FoundComputeShaders);
	OutputDebugStringW(L"\n\tIcons");
	RefreshIcons(FoundIcons);
	OutputDebugStringW(L"\nFinish Loading Assets");
	OutputDebugStringW(L"\n-------------------------------");


}


void MasterEditor::RefreshIcons(std::vector<std::string> FoundIcons)
{
	{
		{// delete removed Icons

			std::set<std::string> CurrentIcons;
			for (auto& IconTexture : m_Icons)
			{
				CurrentIcons.insert(IconTexture.first);
			}

			for (auto& Exisitingshader : FoundIcons)
			{
				std::filesystem::path p(Exisitingshader);
				std::string fileName = TAUtils::WStringToChar(p.stem().c_str());
				CurrentIcons.erase(fileName);
			}

			for (auto& ShaderToDelete : CurrentIcons)
			{
				CurrentIcons.erase(ShaderToDelete);
			}
		}

		for (std::string& IconToProcess : FoundIcons)
		{
			std::filesystem::path p(IconToProcess.c_str());
			std::string fileName = TAUtils::WStringToChar(p.stem().c_str());
			if (!m_Icons.contains(fileName) || m_Icons[fileName] == nullptr)
			{
				m_Icons[fileName] = std::make_unique<Texture2D>(GetRenderer());
			}

			m_Icons[fileName]->SetTexturePath(IconToProcess);
			m_Icons[fileName]->LoadReload();
		}
	}

}

class Texture2D* MasterEditor::GetIcon(std::string IconName)
{
	if (m_Icons.contains(IconName) && m_Icons[IconName] != nullptr)
	{
		return m_Icons[IconName].get();
	}
	return nullptr;
}

void MasterEditor::AddSetActiveTool(EditorToolBase* NewTool)
{

	if (NewTool->GetToolType() == EditorToolBase::ToolType::ToolType_NoneUnique) {
		m_ActiveTools.push_back(NewTool);
		return;
	}
	for (INT32 i = 0; i < m_ActiveTools.size(); i++)
	{
		EditorToolBase* Tool = m_ActiveTools[i];
		if (Tool->GetToolType() != EditorToolBase::ToolType::ToolType_NoneUnique)
		{
			if (Tool->GetToolType() == EditorToolBase::ToolType::ToolType_UniqueSubmissive)
			{
				m_ActiveTools[i] = NewTool;
				return;
			}
		}
	}
	m_ActiveTools.push_back(NewTool);
}

const std::vector<std::unique_ptr<EditorToolBase>>& MasterEditor::GetTools()
{
	return m_EditorTools;
}

UINT64 MasterEditor::KeyStateToUniqueKey(UINT32 ModifierState, UINT32 Key)
{
	return (UINT64)ModifierState + ((UINT64)Key << 32);
}

bool MasterEditor::IsToolActive(EditorToolBase* ToolToCheck) const
{
	for (EditorToolBase* tool : m_ActiveTools)
	{
		if (tool == ToolToCheck) { return true; }
	}
	return false;
}

void MasterEditor::AddToolShortcut(UINT64 Key, EditorToolBase* Tool)
{
	m_ToolsShortcut[Key] = Tool;
}

void MasterEditor::AddToolsShortcutOnDown(UINT64 Key, EditorToolBase* Tool)
{
	m_ToolsShortcutOnDown[Key] = Tool;
}

void MasterEditor::ClearActiveLayer(int NewClearState)
{
	Layer* later = m_ActiveProject->GetSelectedLayer();
	RenderTarget* CT = later->GetCanvasTexture();
	IM_Math::float3& FC = GetForegroundColor();

	switch (NewClearState)
	{
	case 1:
		CT->Clear(FC.x, FC.y, FC.z, 1.0f,GetRenderer());
		break;
	case -1:
		CT->Clear(0, 0, 0, 0.0f, GetRenderer());
		break;
	default:
		break;
	}

}

void MasterEditor::LoadTools()
{
	m_EditorTools.push_back(std::make_unique<MoveTool>(this));
	m_EditorTools.push_back(std::make_unique<CanvasMoveTool>(this));
	m_EditorTools.push_back(std::make_unique<BrushTool>(this));
	m_EditorTools.push_back(std::make_unique<EditorShortcuts>(this));
	AddSetActiveTool(m_EditorTools[m_EditorTools.size() - 1].get());

	for (auto& tool : m_EditorTools)
	{
		tool->Setup();
	}

}
