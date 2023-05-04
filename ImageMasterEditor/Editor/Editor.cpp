#include "Editor.h"

MasterEditor::MasterEditor(std::wstring RootPath, HINSTANCE hInstance)
{
	m_RootPath = RootPath;
	m_Window = std::make_unique<Window>(hInstance);
	m_Window->Init(1280, 720);
	m_Window->ShowTheWindow();

	m_Renderer = std::make_unique<Renderer>();
	m_Renderer->Init(IM_Math::Int2(1280, 720), m_Window.get());

	m_MainWindowUI = std::make_unique<MainWindowUI>(m_Window.get(),this);

	m_BrushManager = std::make_unique<BrushManager>(this);

	RefreshAssets();

#ifdef _DEBUG
	// Just add a default project just so we don't start on an empty page
	AddProject(std::string("Editor test"), IM_Math::Int2(512,512));
#endif // DEBUG

}



void MasterEditor::StartBlockingLoop()
{
	OutputDebugStringA(">>>>  Start App Loop ");
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
			UpdateState();
			m_ActiveProject->CompositeRender();
			m_Renderer->UpdateCamera(m_ActiveProject->GetCameraData());
			m_Renderer->SetOutputRT(m_ActiveProject->GetOutputRT());
			if (!IsInModalState)
			{
				m_BrushManager->Tick((float)m_deltaTime);
			}

			m_Renderer->DrawViewMesh(m_Window.get());
			m_Renderer->SetRenderSize(m_ActiveProject->GetSize() * m_ActiveProject->GetZoom());
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
	m_Projects.push_back(std::make_unique<ImageProject>(ProjectName, CanvasSize, m_Renderer.get()));
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

BrushManager* MasterEditor::GetBrushManager()
{
	return m_BrushManager.get();
}

void MasterEditor::SetActiveProject(ImageProject* ProjectToSetAsActive)
{
	m_ActiveProject = ProjectToSetAsActive; 
	if (m_ActiveProject) { m_Window->SetTitle("Image Master - " + m_ActiveProject->GetProjectName()); }
	else { m_Window->SetTitle("Image Master"); }
}

bool MasterEditor::DrawUI()
{
	return m_MainWindowUI->DrawUI();
}

void MasterEditor::Behaviors(float DeltaTime)
{
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


	// ----------------- Active project -----------
	if (m_ActiveProject == nullptr) { return; }

	// move m_ActiveProject Camera
	IM_Math::float2 CurrentOffset = m_ActiveProject->GetCameraOffset();
	{
		const float canvasSpeed = 1;
		CurrentOffset.x += m_Window->IsKeyDown(Window::KeyCode::A) ? canvasSpeed * DeltaTime : 0;
		CurrentOffset.x -= m_Window->IsKeyDown(Window::KeyCode::D) ? canvasSpeed * DeltaTime : 0;
		CurrentOffset.y += m_Window->IsKeyDown(Window::KeyCode::W) ? canvasSpeed * DeltaTime : 0;
		CurrentOffset.y -= m_Window->IsKeyDown(Window::KeyCode::S) ? canvasSpeed * DeltaTime : 0;
	}

	{
		if (!IsMovingMouse && m_Window->OnMouseDown(2))
		{
			m_MouseMoveLoc = IM_Math::float2((float)m_Window->GetMouseX(), (float)m_Window->GetMouseY());
			IsMovingMouse = true;
		}
		else if (m_Window->OnMouseUp(2))
		{
			IsMovingMouse = false;
		}
		IM_Math::float2 MouseOffset(0, 0);
		if (IsMovingMouse)
		{
			MouseOffset = m_MouseMoveLoc - IM_Math::float2((float)m_Window->GetMouseX(), (float)m_Window->GetMouseY());
			m_MouseMoveLoc = IM_Math::float2((float)m_Window->GetMouseX(), (float)m_Window->GetMouseY());
		}
		CurrentOffset += MouseOffset;
	}

	const float offset = (float)m_Window->MouseScroll();
	const float ZoomStep = 2.0f;
	if (offset > 0)
	{
		m_ActiveProject->SetZoom(m_ActiveProject->GetZoom() / ZoomStep);
		CurrentOffset -= GetMouseCanvasPosition() * (m_ActiveProject->GetZoom() *.5f);
		//OutputDebugStringW((L"\n Zoom1:" +std::to_wstring(m_ActiveProject->GetZoom()) + L"\n").c_str());

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
	IM_Math::float2 CamPos = GetActiveProject()->GetCameraOffset();
	m_MouseCanvasPosition = IM_Math::float2((float)(CamPos.x + m_Window->GetMouseX()), (float)(CamPos.y + m_Window->GetMouseY()));
}


void MasterEditor::RefreshAssets()
{
	OutputDebugStringW(L"\n----- Loading Assets ---- \n");
	OutputDebugStringW(L"--- Reading Shaders");

	std::wstring ContentFolder = m_RootPath + L"\\Content";
	std::wstring ShaderFolder = ContentFolder + L"\\Shaders";
	std::wstring ComputeShaderFolder = ContentFolder + L"\\ComputeShaders";

	std::vector<std::wstring> FoundShaders;
	for (const auto& entry : std::filesystem::directory_iterator(ShaderFolder))
	{
		FoundShaders.push_back(entry.path().c_str());

		std::wstring FoundShader(L"\n>>> Shader :\t");
		FoundShader += entry.path().c_str();
		OutputDebugStringW(FoundShader.c_str());
	}

	std::vector<std::wstring> FoundComputeShaders;
	for (const auto& entry : std::filesystem::directory_iterator(ComputeShaderFolder))
	{
		FoundComputeShaders.push_back(entry.path().c_str());

		std::wstring FoundShader(L"\n>>> ComputeShader :\t");
		FoundShader += entry.path().c_str();
		OutputDebugStringW(FoundShader.c_str());
	}

	OutputDebugStringW(L"\n--- Loading Shaders\n");
	m_Renderer->RefreshShaders(FoundShaders, FoundComputeShaders);
	OutputDebugStringW(L"--- Shaders Done \n");
	OutputDebugStringW(L"----- Finish Loading Assets ---- \n");


}


void MasterEditor::Action_New()
{


}

