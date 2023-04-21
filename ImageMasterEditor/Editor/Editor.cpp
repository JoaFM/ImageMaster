#include "Editor.h"

MasterEditor::MasterEditor(std::wstring RootPath, HINSTANCE hInstance)
{
	m_RootPath = RootPath;
	m_Window = std::make_unique<Window>(hInstance);
	m_Window->Init(1280, 720);
	m_Window->ShowTheWindow();

	m_Renderer = std::make_unique<Renderer>();
	m_Renderer->Init(IM_Math::Int2(1280, 720), m_Window.get());

	m_MainWindowUI = std::make_unique<MainWindowUI>(m_Window.get());

	RefreshAssets();

#ifdef _DEBUG
	AddProject(std::wstring(L"Editor test"));
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
			m_Renderer->UpdateCamera(m_ActiveProject->GetCameraData());
			m_Renderer->SetOutputRT(m_ActiveProject->GetOutputRT());
		}
		// UI
		DrawUI();
		

		// Calc and present
		m_Renderer->Present(m_Window.get());

		// Windows message loop
		m_Window->PumpWindowMessages();

		// Input
		Behaviors();
	}
}

void MasterEditor::AddProject(std::wstring ProjectName)
{
	std::wstring rndS = TAUtils::CharToWString(TAUtils::RandomString(6).c_str());
	m_Projects.push_back(std::make_unique<ImageProject>(ProjectName + L"____" + rndS,IM_Math::Int2(512,512),m_Renderer.get()));
	ActiveProjectIndex = (INT32)(m_Projects.size() - 1);
	m_ActiveProject = m_Projects[ActiveProjectIndex].get();
}

void MasterEditor::DrawUI()
{
	m_MainWindowUI->DrawUI(m_ActiveProject, this);
}

void MasterEditor::Behaviors()
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
			m_ActiveProject = m_Projects[ActiveProjectIndex].get();
		}
		else 
		{
			m_ActiveProject = nullptr;
			ActiveProjectIndex = -1;
		}
	}


	// ----------------- Active project -----------
	if (m_ActiveProject == nullptr) { return; }

	// move m_ActiveProject Camera

	IM_Math::float2 CurrentOffset = m_ActiveProject->GetCameraOffset();
	CurrentOffset.x += m_Window->IsKeyDown(Window::KeyCode::A) ? 10 : 0;
	CurrentOffset.x -= m_Window->IsKeyDown(Window::KeyCode::D) ? 10 : 0;
	CurrentOffset.y += m_Window->IsKeyDown(Window::KeyCode::W) ? 10 : 0;
	CurrentOffset.y -= m_Window->IsKeyDown(Window::KeyCode::S) ? 10 : 0;
	m_ActiveProject->SetCameraOffset(CurrentOffset);


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
		std::wstring FoundShader(L"\n>>> Shader :\t");
		FoundShaders.push_back(entry.path().c_str());
		FoundShader += entry.path().c_str();
		OutputDebugStringW(FoundShader.c_str());
	}

	std::vector<std::wstring> FoundComputeShaders;
	for (const auto& entry : std::filesystem::directory_iterator(ComputeShaderFolder))
	{
		std::wstring FoundShader(L"\n>>> ComputeShader :\t");
		FoundComputeShaders.push_back(entry.path().c_str());
		FoundShader += entry.path().c_str();
		OutputDebugStringW(FoundShader.c_str());
	}

	OutputDebugStringW(L"\n--- Loading Shaders\n");
	m_Renderer->RefreshShaders(FoundShaders, FoundComputeShaders);
	OutputDebugStringW(L"--- Shaders Done \n");
	OutputDebugStringW(L"----- Finish Loading Assets ---- \n");


}

