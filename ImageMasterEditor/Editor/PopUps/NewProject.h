#pragma once
#include "MainApp/Default_Headers.h"
#include <UXUI/Window.h>
#include "PopUp.h"

class Popup_NewProject : public PopupBase
{
	
public:
	virtual bool UI_Tick(class MasterEditor* MainEditor) override;
	int ProjectSize[2] = { 512,512 };
	char ProjectName[100] = "No Name ";

};
