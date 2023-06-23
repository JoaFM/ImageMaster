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
	void UI_DrawLayer(class MainWindowUI * MainUI);
	bool IsSelected() const;
	void ReadBackData(char* Filepath);
	class RenderTarget* GetCanvasTexture() { return m_CanvasTexture.get(); }

	void Clear();
private:
	// Project description
	std::wstring m_BlendMode;
	std::string m_LayerName;
	std::string m_ID;
	//IM_Math::Int2 m_LayerSize;
	int m_CurrentBlendMode = 1;
	std::unique_ptr<RenderTarget> m_CanvasTexture = nullptr;
	
	RenderTypes::CB_Layer_Struct m_RenderData;

	//ref
	class ImageProject* m_ParentProject = nullptr;


public:
	int& GetCurrentItemIndex();
};
