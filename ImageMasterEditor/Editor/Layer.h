#pragma once
#include "MainApp/Default_Headers.h"
#include "Utils/IM_Math.h"
#include "Engine/Renderer/RenderTarget.h"

class Layer
{
public:
	Layer(std::string LayerName, class ImageProject* ParentProject);
	Layer() {};
	
	void UI_DrawLayer();
	bool IsSelected() const;
private:
	// Project description
	std::wstring m_BlendMode;
	std::string m_LayerName;
	IM_Math::Int2 m_LayerSize;
	IM_Math::Int2 m_Offset;

	std::unique_ptr<RenderTarget> m_LayerData = nullptr;
	
	//ref
	class ImageProject* m_ParentProject = nullptr;


};
