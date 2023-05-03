#pragma once
#include "MainApp/Default_Headers.h"
#include "Utils/IM_Math.h"

class BrushManager
{
public:
	BrushManager(class MasterEditor* MainEditor);
	void Tick(float DeltaTime);
	void DrawBrushUI() ;
	void DrawColorUI();
private:
	class MasterEditor* m_MainEditor = nullptr;
	class Window* m_Window = nullptr;
	class Renderer* m_Renderer = nullptr;

	float m_BrushTraveledDistance = 0;
	float m_BrushSpacing = 20;
	float m_BrushSize = 20;
	float m_BrushAlpha = .1f;
	bool m_IsDrawing = false;
	IM_Math::float2 m_LastLocation;
	IM_Math::float2 m_ThisLocation;
	IM_Math::float3 m_Color;


	void StartDarwing();
	void EndDrawing();
	void UpdateDrawing();
	void DrawBrushAtCurrentLocation();
};
