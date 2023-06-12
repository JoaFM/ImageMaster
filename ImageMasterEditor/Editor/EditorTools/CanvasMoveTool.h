#pragma once
#include "EditorToolBase.h"


class CanvasMoveTool : public EditorToolBase
{
public:
	CanvasMoveTool(class MasterEditor* Editor);

	virtual void UI_DrawToolbarIcon() override;
	virtual void UI_DrawToolSettings() override;
	virtual void Tick(float deltaTime) override;


	virtual UINT64 GetShortcut() override;
	virtual UINT64 GetShortcutWhilePressed() override;

private:
	bool IsMovingMouse = false;
	IM_Math::float2 m_MouseMoveLoc;
};
