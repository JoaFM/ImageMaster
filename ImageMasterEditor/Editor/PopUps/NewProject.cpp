#include "NewProject.h"
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_internal.h"
#include "../Editor.h"

bool Popup_NewProject::UI_Tick(class MasterEditor* MainEditor)
{
	ImGui::OpenPopup("NewProjectPopUp");
	bool IsPopUpOpen = false;
	
	if (ImGui::BeginPopupModal("NewProjectPopUp", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoTitleBar
	))
	{
		IsPopUpOpen = true;
		ImGui::Text("NewProject !\n\n");
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(.12f, .12f, .12f, 1));
		ImGui::Text("Image Name"); ImGui::SameLine(); ImGui::InputText("##ImageName", ProjectName, 100);;
		ImGui::Text("Image Size"); ImGui::SameLine(); ImGui::InputInt2("##ImageSize", ProjectSize);
		ImGui::PopStyleColor();
		ImGui::NewLine();

		ImGui::Separator();
		ImGui::NewLine();
		{// buttons
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				MainEditor->AddProject(std::string(ProjectName), IM_Math::Int2(ProjectSize[0], ProjectSize[1]));
				ImGui::CloseCurrentPopup();
				IsPopUpOpen = false;
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				IsPopUpOpen = false;
			}
		}
		ImGui::EndPopup();
	}
	return IsPopUpOpen;
}
