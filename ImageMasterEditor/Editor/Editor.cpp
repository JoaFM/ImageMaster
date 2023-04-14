#include "Editor.h"



void MasterEditor::Start(HINSTANCE hInstance)
{
	m_Window = std::make_unique<Window>(hInstance);
	m_Window->Init(1280, 720);
	m_Window->ShowTheWindow();

	m_Renderer = std::make_unique<Renderer>();
	m_Renderer->Init(IM_Math::Int2(1280, 720), m_Window.get());
}

void MasterEditor::StartBlockingLoop()
{
	OutputDebugStringA("\n  StartBlockingLoop \n\n");
	bool Quiting = false;
	auto t_start = std::chrono::high_resolution_clock::now();
	auto t_end = std::chrono::high_resolution_clock::now();

	while (!Quiting && m_Window)
	{
		t_end = std::chrono::high_resolution_clock::now();
		m_deltaTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		t_start = std::chrono::high_resolution_clock::now();

		m_Renderer->Tick();
		//m_Renderer->ReadyNextFrame();

		//m_World->Tick((float)deltaTime);
		//m_Renderer->PresentFrame();
		m_Window->PumpWindowMessages();
		Quiting = m_Window->OnKeyDown(Window::KeyCode::Escape);
		//find a nice enum under C:\Users\Joama\source\repos\JoaFM\TADrawFramework\TADraw\Objects\Camera.cs 183
	}
}


