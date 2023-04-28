#pragma once
#include "MainApp/Default_Headers.h"
#include "Utils/IM_Math.h"
#include "Engine/Renderer/RenderTarget.h"

class Layer
{
public:
	Layer(std::string LayerName, class ImageProject* ParentProject);
	Layer() {};
	
	void Composite(class RenderTarget* OutputRT);
	void UI_DrawLayer();
	bool IsSelected() const;
	class RenderTarget* GetCanvasTexture() { return m_CanvasTexture.get(); }
private:
	// Project description
	std::wstring m_BlendMode;
	std::string m_LayerName;
	std::string m_ID;
	IM_Math::Int2 m_LayerSize;
	int currentItem = 0;
	std::unique_ptr<RenderTarget> m_CanvasTexture = nullptr;
	
	//ref
	class ImageProject* m_ParentProject = nullptr;


public:
	int& GetCurrentItemIndex();
};
