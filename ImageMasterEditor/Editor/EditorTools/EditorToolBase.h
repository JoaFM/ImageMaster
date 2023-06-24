#pragma once
#include "pch.h"
#include <intsafe.h>


class EditorToolBase
{
public:

	EditorToolBase(class MasterEditor* Editor);
	void Setup();
	virtual void UI_DrawToolbarIcon();
	virtual void UI_DrawToolSettings();
	virtual void Tick(float deltaTime) {};
	virtual void UI_DrawOverlay(class ImageProject* CurrProject) {};

	virtual UINT64 GetShortcut() { return 0; };
	virtual UINT64 GetShortcutWhilePressed() { return 0; };

	enum class ToolType {
		ToolType_NoneUnique,// Have as many as you want
		ToolType_UniqueSubmissive,// If another tool comes around it overrides this one
		//ToolType_UniqueDominant, // If another tool come around this one will survive and the tool will get rejected
	};

	ToolType GetToolType() { return m_ToolType; }
protected:
	class MasterEditor* GetEditor() { return m_MainEditor; }
	class Renderer* GetRenderer() { return m_Renderer; }
	class Window* GetWindow() { return m_Window; }

	void SetToolType(ToolType ToolTypeState) { m_ToolType = ToolTypeState; }
private:
	class MasterEditor* m_MainEditor = nullptr;
	class Window* m_Window = nullptr;
	class Renderer* m_Renderer = nullptr;
	ToolType m_ToolType = ToolType::ToolType_UniqueSubmissive;

public:
};
