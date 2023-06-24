#pragma once
#include "pch.h"
#include <UXUI/Window.h>
#include "PopUp.h"

class Popup_SaveLayer : public PopupBase
{
	
public:
	virtual bool UI_Tick(class MasterEditor* MainEditor) override;
	char Filepath[1000] = {"d:\\test.png"};
	class Layer* LayerToSave = nullptr;
};
