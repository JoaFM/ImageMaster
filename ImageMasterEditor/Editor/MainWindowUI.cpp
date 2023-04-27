
#include "MainWindowUI.h"
#include "ImageProject.h"
#include "Utils/IM_STD.h"
#include "Editor.h"

MainWindowUI::MainWindowUI(class Window* ParentWindow, class MasterEditor* Editor)
	:
	m_ParentWindow(ParentWindow),
	m_Editor(Editor)
{
	{// IMGUI context
		Renderer* renderer = m_Editor->GetRenderer();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(ParentWindow->GetHWND());
		ImGui_ImplDX11_Init(renderer->GetDevice(), renderer->GetDeviceContext());
	}

	{// Add font
		ImGuiIO& io = ImGui::GetIO();
		std::string FP = TAUtils::WStringToChar((m_Editor->GetRootPath() + L"\\Content\\Fonts\\CaviarDreams.ttf").c_str());
		io.Fonts->AddFontDefault();
		m_mainFont = io.Fonts->AddFontFromFileTTF(FP.c_str(), 14.0f);
		io.Fonts->Build();
		IM_ASSERT(m_mainFont != nullptr);
	}

	UI_SetGlobalStyle();

}

void MainWindowUI::DrawUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	UI_DrawLayer();
	UI_DrawAppMenuBar();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void MainWindowUI::UI_DrawLayer()
{
	ImGui::NewFrame();
	ImGui::PushFont(m_mainFont);
	ImGui::SetNextWindowSize(ImVec2(200, 500), 1);

	if (ImGui::Begin("Layers"))
	{
		// Get blend Options
		std::vector<std::string> LayerModes = m_Editor->GetActiveProject()->GetLayerModesAsString();
		char** Items = new char* [LayerModes.size()];
		const char eol = '\0';
		for (INT32 i = 0; i < LayerModes.size(); i++)
		{
			Items[i] = new char[LayerModes[i].size() + 2];
			std::memcpy(Items[i], LayerModes[i].c_str(), LayerModes[i].size());
			std::memcpy(&Items[i][LayerModes[i].size()], &eol, 1);
		}
		// Blend mode combo area
		int StandInIndex = 0;
		Layer* ActiveLayer = m_Editor->GetActiveProject()->FindActiveLayer();
		if (ActiveLayer)
		{
			ImGui::Combo("Mode##", &ActiveLayer->GetCurrentItemIndex(), Items, (INT32)LayerModes.size());
		}
		else
		{
			ImGui::Combo("Mode##", &StandInIndex,"---\0\0");
		}

		// Draw layers
		if (ImGui::BeginListBox("  Layers", ImVec2(188, -1)))
		{
			for (INT32 i = (UINT)m_Editor->GetActiveProject()->GetLayers().size() - 1; i >= 0; i--)
			{
				m_Editor->GetActiveProject()->GetLayers()[i]->UI_DrawLayer();
			}
		}
		ImGui::EndListBox();

		// clean up blend mode options
		for (INT32 i = 0; i < LayerModes.size(); i++)
		{
			delete[] Items[i];
		}
		delete[] Items;
	}

	ImGui::PopFont();

	ImGui::End();
}

void MainWindowUI::UI_SetGlobalStyle()
{
	ImGui::StyleColorsDark();
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
		ImGui::PushFont(m_mainFont);

		if (ImGui::BeginMenu("File"))
		{

			ImageProject::UI_FileMenueUI(m_Editor);
			ImGui::EndMenu();
		}
		if (m_Editor->GetActiveProject() != nullptr)
		{
			std::string Title = std::string("\t\t\t[ ") + m_Editor->GetActiveProject()->GetProjectName() + " ]";
			ImGui::Text(Title.c_str());
		}
		else
		{
			ImGui::Text("\t\t\t[ No active project ]");  
		}

		ImGui::PopFont();

		ImGui::EndMainMenuBar();
	}
}

