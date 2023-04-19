


#include "Renderer.h"
#include "UXUI/Window.h"


void Renderer::Init(IM_Math::Int2 size, Window* MainWindow)
{
	//#TODO: Get this data from the window rect
	m_CB_General.RenderBufferSize.x = m_CB_General.DisplayWindowSize.x = (float)size.x;
	m_CB_General.RenderBufferSize.y = m_CB_General.DisplayWindowSize.y = (float)size.y;
	CreateSwapChain(MainWindow);


	//m_testBuffer = std::make_unique<Buffer>();;
	m_testRT = std::make_unique<RenderTarget>();;

	// So if i want to write it needs to be unorderd but then cant updlaod??
	// with is right height is 0 ??
	//m_testBuffer->Setup(16, 128 * 128, Buffer::UNORDERED, GetDevice());

// 	UINT8* data = new UINT8[128 * 128 * 4];
// 
// 	for (INT32 x = 0; x < 128; x++)
// 	{
// 		for (INT32 y = 0; y < 128; y++)
// 		{
// 			INT32 index = (x + (y * 128)) *4;
// 			data[index] = x;
// 			data[index + 1] = y;
// 			data[index + 2] = 0;
// 			data[index + 3] = 255;
// 		}
// 	}
	//m_testBuffer->UpdateBufferData(data, 128 * 128 * 4, GetDeviceContext());
	//delete[] data;
	
	m_testRT->CreateTarget(128, 128, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, this);

	m_testCompute = std::make_unique<ComputeShader>();;
	m_testCompute->SetShaderPath(std::wstring(L"D:/Code/ImageMaster/Content/Shaders/BasicComputeShader.hlsl"));
	m_testCompute->LoadReload(GetDevice());
	m_testCompute->SetRT(m_testRT.get());
	// IMGUI

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(MainWindow->GetHWND());
	ImGui_ImplDX11_Init(GetDevice(), GetDeviceContext());
	ImGui::StyleColorsDark();
}

void Renderer::CreateSwapChain(Window* MainWindow)
{

	if (!m_SwapChain)
	{
		m_SwapChain = std::make_unique<SwapChain>();

		m_SwapChain->CreateSwapChain(
			IM_Math::Int2((INT32)m_CB_General.DisplayWindowSize.x, (INT32)m_CB_General.DisplayWindowSize.y),
			MainWindow,
			this,
			m_Device, 
			m_Device_Context
		);

		InternalSetRenderTarget(m_SwapChain->GetRenderTarget());
	}


}

void Renderer::Tick()
{
	if (ActiveRenderTarget)
	{
		ActiveRenderTarget->Clear(0.1f, 0.2f, 0.6f, 1.0f, this);

		if (m_testCompute->Bind(GetDeviceContext()))
		{
			m_testCompute->Dispatch(GetDeviceContext(), 16, 16, 1);
			m_testCompute->UnBind(GetDeviceContext());
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2(5, 30), 1);
		ImGui::SetNextWindowSize(ImVec2(100, 720), 1);
		
		ImGui::SetNextWindowCollapsed(false, 1);
		ImGui::Begin("Test",0, ImGuiWindowFlags_NoTitleBar);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Hello");

		ImGui::End();

		bool show_demo_window = true;
		bool show_another_window = false;

// 		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
// 		{
// 			static float f = 0.0f;
// 			static int counter = 0;
// 			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// 
// 			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
// 
// 			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
// 			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
// 			ImGui::Checkbox("Another Window", &show_another_window);
// 
// 			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
// 			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
// 
// 			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
// 				counter++;
// 			ImGui::SameLine();
// 			ImGui::Text("counter = %d", counter);
// 
// 			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / 10, 10);
// 			ImGui::End();
// 		}


		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New"))
					{
						//Do something
					}
					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}


		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


		m_SwapChain->Present(1, 0);
	}
}

void Renderer::InternalSetRenderTarget(RenderTarget* NewRT)
{
	ActiveRenderTarget = NewRT;
	ActiveRenderTarget->Bind(this);
}
