
#include "MainWindowUI.h"
#include "ImageProject.h"
#include "Utils/IM_STD.h"
#include "Editor.h"



MainWindowUI::MainWindowUI(class Window* ParentWindow, class MasterEditor* Editor)
	:
	m_ParentWindow(ParentWindow),
	m_Editor(Editor)
{

}

void MainWindowUI::DrawUI()
{
	DrawAppUI();
}

void MainWindowUI::DrawAppUI()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	UI_SetGlobalStyle();
	UI_DrawLayer();
	UI_DrawAppMenuBar();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void MainWindowUI::UI_DrawLayer()
{

	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(200, 500), 1);

	if (ImGui::Begin("Layers"))
	{
		if (ImGui::BeginListBox("  Layers", ImVec2(188, -1)))
		{
			for (INT32 i = (UINT)m_Editor->GetActiveProject()->GetLayers().size() - 1; i >= 0; i--)
			{
				m_Editor->GetActiveProject()->GetLayers()[i]->UI_DrawLayer();
			}
		}
		ImGui::EndListBox();
	}
	ImGui::End();
}

void MainWindowUI::UI_SetGlobalStyle()
{
	ImGuiStyle& CurrentStype = ImGui::GetStyle();
	CurrentStype.FrameRounding = 0;
	CurrentStype.WindowRounding = 0;
	CurrentStype.Colors[ImGuiCol_WindowBg] = ImVec4(.05f, .05f, .05f, 1.0f);
	CurrentStype.Colors[ImGuiCol_FrameBg] = ImVec4(0.04f, .04f, .04f, 0.0f);
	CurrentStype.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.03f, .03f, .03f, 1.0f);
	CurrentStype.Colors[ImGuiCol_Button] = ImVec4(0.1f, .1f, .1f, 1.0f);
	CurrentStype.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.17f, .17f, .17f, 1.0f);
	CurrentStype.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, .25f, .25f, 1.0f);


	CurrentStype.WindowTitleAlign = ImVec2(.5, .5);
	CurrentStype.ScrollbarRounding = 0;
}

void MainWindowUI::UI_DrawAppMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			ImageProject::UI_FileMenueUI(m_Editor);
			ImGui::EndMenu();
		}
		if (m_Editor->GetActiveProject() != nullptr)
		{
			std::string Title = std::string("\t\t\t[ ") + m_Editor->GetActiveProject()->GetProjectName() + " ]";
			ImGui::Text(Title.c_str());               // Display some text (you can use a format strings too)
		}
		else
		{
			ImGui::Text("\t\t\t[ No active project ]");               // Display some text (you can use a format strings too)
		}

		ImGui::EndMainMenuBar();
	}
}

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
