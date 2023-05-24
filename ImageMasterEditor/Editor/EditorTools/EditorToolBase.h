#pragma once
#include "MainApp/Default_Headers.h"


class EditorToolBase
{
public:
	EditorToolBase(class MasterEditor* Editor);
	virtual void UI_DrawToolbarIcon();
	virtual void UI_DrawToolSettings();
	virtual void Tick(float deltaTime) {};

protected:
	class MasterEditor* GetEditor() { return m_MainEditor; }
	class Renderer* GetRenderer() { return m_Renderer; }
	class Window* GetWindow() { return m_Window; }

private:
	class MasterEditor* m_MainEditor = nullptr;
	class Window* m_Window = nullptr;
	class Renderer* m_Renderer = nullptr;
};
