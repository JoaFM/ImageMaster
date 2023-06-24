#include "pch.h"

#include "MainWindowUI.h"
#include "ImageProject.h"
#include "Utils/IM_STD.h"
#include "Editor.h"
#include "External/IMGUI/imgui_internal.h"
#include "PopUps/NewProject.h"

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

bool MainWindowUI::DrawUI()
{

	bool IsInModalState = false;
	std::set<std::string> Messages;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::PushFont(m_mainFont);
	//======================
	UI_DrawDebug();
	//======================

	UI_DrawLayer();
	UI_DrawAppMenuBar(Messages);
	UI_DrawBrushUI();
	UI_DrawColourUI();
	UI_DrawToolSelectorBar();
	IsInModalState = UI_DrawPopUps(Messages);

	//======================
	ImGui::PopFont();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return IsInModalState;
}


void MainWindowUI::UI_DrawLayer()
{
	const float SpaceFromTop = 200;
	if (!m_Editor->GetActiveProject()) return;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	// 
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x -200, ImGui::GetFrameHeight() + SpaceFromTop));

	// Add menu bar flag and disable everything else
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoDecoration |
		//ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus// |
		//ImGuiWindowFlags_NoBackground
		;// |
		//ImGuiWindowFlags_MenuBar;


	

	ImGui::SetNextWindowSize(ImVec2(200, viewport->Size.y - (SpaceFromTop + ImGui::GetFrameHeight())), 1);

	if (ImGui::Begin("Layers",nullptr, flags))
	{

		ImGui::Text("Layers");
		
		ImGui::SameLine();
		if (ImGui::SmallButton("+##LayerButton"))
		{
			TAUtils::Log("Add Layer");
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("-##layerButton"))
		{
			TAUtils::Log("Remove Layer");
		}

		ImGui::Separator();
		ImGui::NewLine();

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
				m_Editor->GetActiveProject()->GetLayers()[i]->UI_DrawLayer(this);
			}
			ImGui::EndListBox();
		}

		// clean up blend mode options
		for (INT32 i = 0; i < LayerModes.size(); i++)
		{
			delete[] Items[i];
		}
		delete[] Items;

		ImGui::End();
	}

	
	
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

void MainWindowUI::UI_DrawAppMenuBar(std::set<std::string>& Messages)
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::PushFont(m_mainFont);

		if (ImGui::BeginMenu("File"))
		{

			ImageProject::UI_FileMenuNewUI(Messages);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			auto& Projects = m_Editor->GetProjects();
			for (auto& project : Projects)
			{
				project->UI_FileMenuWindowUI();
			}
			ImGui::EndMenu();
		}

		ImGui::PopFont();

		ImGui::EndMainMenuBar();
	}


}

void MainWindowUI::UI_DrawDebug()
{
	if (ImGui::Begin("Debug_Menu"))
	{
		ImGui::Text("Canvas X: %f y:%f", m_Editor->GetMouseCanvasPosition().x, m_Editor->GetMouseCanvasPosition().y);
		ImGui::Text("Color %f, %f, %f, %f", m_Editor->GetUnderMousePixel().x, m_Editor->GetUnderMousePixel().y, m_Editor->GetUnderMousePixel().z, m_Editor->GetUnderMousePixel().w);
	}
	ImGui::End();

}

void MainWindowUI::UI_DrawBrushUI()
{
 	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(45, ImGui::GetFrameHeight()));
 	ImGui::SetNextWindowSize(ImVec2(200,200));
 
 	// Add menu bar flag and disable everything else
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoDecoration |
		//ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus// |
		//ImGuiWindowFlags_NoBackground
		;// |
 		//ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin("Tool Bar", nullptr, flags))
	{
		for (EditorToolBase* Tool : m_Editor->GetActiveTool())
		{
			Tool->UI_DrawToolSettings();
		}
	}
	ImGui::End();

}

bool MainWindowUI::UI_DrawPopUps(std::set<std::string>& Messages)
{
	if (Messages.contains("NewProjectPopUp")) {
		CurrentPopup = std::make_unique<Popup_NewProject>();
	}
	if (CurrentPopup)
	{
		if (!CurrentPopup->UI_Tick(m_Editor))
		{
			CurrentPopup.reset();
		}
		else
		{
			return true;
		}
	}
	return false;
}

void MainWindowUI::UI_DrawToolSelectorBar()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));
	ImGui::SetNextWindowSize(ImVec2(45, viewport->Size.y - ImGui::GetFrameHeight()));

	// Add menu bar flag and disable everything else
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoDecoration |
		//ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus// |
		//ImGuiWindowFlags_NoBackground
		;// |
		//ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin("Tool Selection", nullptr, flags))
	{
		ImGui::Text("Tools");
		ImGui::Separator();
		ImGui::NewLine();
		
		for (const auto& tool: m_Editor->GetTools())
		{
			tool->UI_DrawToolbarIcon();
		}
	}
	ImGui::End();


}

 void MainWindowUI::UI_DrawColourUI()
 {

	 const ImGuiViewport* viewport = ImGui::GetMainViewport();

	 // Add menu bar flag and disable everything else
	 ImGuiWindowFlags flags =
		 ImGuiWindowFlags_NoDecoration |
		 //ImGuiWindowFlags_NoInputs |
		 ImGuiWindowFlags_NoMove |
		 //ImGuiWindowFlags_NoScrollWithMouse |
		 ImGuiWindowFlags_NoSavedSettings |
		 ImGuiWindowFlags_NoBringToFrontOnFocus// |
		 //ImGuiWindowFlags_NoBackground
		 ;// |
		 //ImG

	 ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x - 200, ImGui::GetFrameHeight()));
	 ImGui::SetNextWindowSize(ImVec2(200, 200));
	 if (ImGui::Begin("Color##Window", nullptr, flags))
	 {

		 float* Col = reinterpret_cast<float*>(&m_Editor->GetForegroundColor());
		 ImGui::ColorEdit3("Foreground", Col, ImGuiColorEditFlags_NoInputs);
		 ImGui::Separator();
		 ImGui::NewLine();

		 int num = 10;
		 for (int i = 0; i < m_Swatches.size(); i++)
		 {
			 if (num < 5) { ImGui::SameLine(); }
			 else { (num = 0); }

			 if (ImGui::ColorButton((std::string("Swatch ") + std::to_string(i)).c_str(),
				 ImVec4(
					 powf(m_Swatches[i].x, 2.2f),
					 powf(m_Swatches[i].y, 2.2f),
					 powf(m_Swatches[i].z, 2.2f),
					 1)))
			 {
				 m_Editor->GetForegroundColor() = m_Swatches[i];
			 }
			 num++;
		 }
	 }
	 ImGui::End();
 }
