#include "pch.h"
#include "SaveLayer.h"
#include "External/IMGUI/imgui.h"
#include "External/IMGUI/imgui_internal.h"
#include "../Editor.h"

bool Popup_SaveLayer::UI_Tick(class MasterEditor* MainEditor)
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
		ImGui::Text("Save layer !\n\n");
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(.12f, .12f, .12f, 1));
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Text("File Path"); ImGui::SameLine(); ImGui::InputText("##ImageName", Filepath, 1000);;
		ImGui::PopStyleColor();
		ImGui::NewLine();

		ImGui::Separator();
		ImGui::NewLine();
		{// buttons
			ImGui::Indent(100);
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				LayerToSave->ReadBackData(Filepath);
				ImGui::CloseCurrentPopup();
				IsPopUpOpen = false;
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			ImGui::Indent(100);

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				IsPopUpOpen = false;
			}
			ImGui::Indent(100);

		}
		ImGui::EndPopup();
	}
	return IsPopUpOpen;
}
