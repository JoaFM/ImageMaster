#pragma once
#include "EditorToolBase.h"


class MoveTool : public EditorToolBase
{
public:
	MoveTool(class MasterEditor* Editor);

	virtual void UI_DrawToolbarIcon() override;
	virtual void UI_DrawToolSettings() override;
	virtual void Tick(float deltaTime) override;
	virtual UINT64 GetShortcut() override;

};
