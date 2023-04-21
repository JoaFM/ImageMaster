
#include "MainWindowUI.h"
#include "ImageProject.h"
#include "Utils/IM_STD.h"



MainWindowUI::MainWindowUI(class Window* ParentWindow)
	:
	m_ParentWindow(ParentWindow)
{

}

void MainWindowUI::DrawUI(ImageProject* ActiveProject, class MasterEditor* Editor)
{
	DrawAppUI(ActiveProject, Editor);
}

void MainWindowUI::DrawAppUI(ImageProject* ActiveProject, class MasterEditor* Editor)
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(5, 30), 1);
	ImGui::SetNextWindowSize(ImVec2(100, 720), 1);

	ImGui::SetNextWindowCollapsed(false, 1);
	ImGui::Begin("Test", 0, ImGuiWindowFlags_NoTitleBar);
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Hello");

	ImGui::End();

	bool show_demo_window = true;
	bool show_another_window = false;

	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{

				ImageProject::UI_FileMenueUI(Editor);
				ImGui::EndMenu();
			}
			if (ActiveProject != nullptr)
			{
				std::wstring Title = std::wstring(L"\t\t\t[ ") + ActiveProject->GetProjectName() + L" ]";
				ImGui::Text(TAUtils::WStringToChar(Title.c_str()).c_str());               // Display some text (you can use a format strings too)
			}
			else
			{
				ImGui::Text("\t\t\t[ No active project ]");               // Display some text (you can use a format strings too)
			}
			
			ImGui::EndMainMenuBar();
		}
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


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

}
