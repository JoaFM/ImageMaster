#pragma once
#include "EditorToolBase.h"


class BrushTool : public EditorToolBase
{
public:
	BrushTool(class MasterEditor* Editor);

	virtual void UI_DrawToolbarIcon() override;
	virtual void UI_DrawToolSettings() override;
	virtual void Tick(float deltaTime) override;
	virtual UINT64 GetShortcut() override;

private:

	float m_BrushTraveledDistance = 0;
	float m_BrushSpacing = 0.1f;
	float m_BrushSize = 10;
	float m_BrushAlpha = 0.1f;
	bool m_IsDrawing = false;
	IM_Math::float2 m_LastMouseLocation;
	IM_Math::float2 m_BrushLocation;
	IM_Math::float2 m_CurrentMouseLocation;

	std::vector<IM_Math::float3> m_Swatches = {
		{0,0,0},{1,1,1},{1,0,0},{0,1,0},{0,0,1},
		{1,1,0},{0,1,1},{1,0,1},{.025f,.025f,.025f},{.05f,.05f,.05f},
		{.1f,.1f,.1f},{.2f,.2f,.2f},{.3f,.3f,.3f},{.4f,.4f,.4f},{.5f,.5f,.5f},
		{.6f,.6f,.6f},{.7f,.7f,.7f},{.8f,.8f,.8f},{.9f,.9f,.9f},

	};

	void StartDarwing();
	void EndDrawing();
	void UpdateDrawing();
	void DrawBrushAtLocation(IM_Math::float2 BrushLocation);

};
