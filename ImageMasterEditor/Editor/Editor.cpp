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

	RefreshAssets();

#ifdef _DEBUG
	// Just add a default project just so we don't start on an empty page
	AddProject(std::string("Editor test"));
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

		m_ActiveProject->CompositeRender();
		if (m_ActiveProject)
		{
			UpdateState();
			m_Renderer->UpdateCamera(m_ActiveProject->GetCameraData());
			m_Renderer->SetRenderSize(m_ActiveProject->GetSize());
			m_Renderer->SetOutputRT(m_ActiveProject->GetOutputRT());
			DoAction();
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

void MasterEditor::AddProject(std::string ProjectName)
{
	std::string rndS =TAUtils::RandomString(6).c_str();
	m_Projects.push_back(std::make_unique<ImageProject>(ProjectName + "____" + rndS,IM_Math::Int2(512,512),m_Renderer.get()));
	ActiveProjectIndex = (INT32)(m_Projects.size() - 1);
	m_ActiveProject = m_Projects[ActiveProjectIndex].get();
}

void MasterEditor::DrawUI()
{
	m_MainWindowUI->DrawUI();
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

void MasterEditor::UpdateState()
{
	IM_Math::float2 CamPos = GetActiveProject()->GetCameraOffset();
	m_MouseCanvasPosition = IM_Math::Int2((INT32)(CamPos.x + m_Window->GetMouseX()), (INT32)(CamPos.y + m_Window->GetMouseY()));
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

void MasterEditor::DoAction()
{
	bool mouse0 = m_Window->IsMouseDown(0);
	bool mouse2 = m_Window->IsMouseDown(1);
	if (!(mouse0 || mouse2)) { return; }


		//Mouse
	RenderTypes::CB_BrushInput_Struct BrushInput;
	BrushInput.MouseButton = 1;
	BrushInput.MousePosition = IM_Math::float2((float)GetMouseCanvasPosition().x, (float)GetMouseCanvasPosition().y);
	BrushInput.PAD0 = IM_Math::float2();
	BrushInput.BrushMainColour = mouse0 ? IM_Math::float3(1, 0, 0) : IM_Math::float3(0, 1, 0);

	m_Renderer->GetConstantBuffers()[RenderTypes::ConstanBuffer::CB_BrushInput]->UpdateData(&BrushInput);

	if (Layer* alayer = m_ActiveProject->GetSelectedLayer())
	{
		ComputeShader* BlendCP = m_Renderer->GetComputeShaders()[L"Brush_Circle"].get();

		BlendCP->SetTexture("BufferOut", alayer->GetCanvasTexture());
		BlendCP->SetTexture("CanvasTexture", nullptr);
		if (m_Renderer->BindComputeShader(L"Brush_Circle"))
		{
			BlendCP->Dispatch(m_Renderer->GetDeviceContext());
			m_Renderer->UnbindCurrentComputeShader();
		}
		
	}

}
