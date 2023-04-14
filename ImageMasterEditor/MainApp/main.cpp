#include "Default_Headers.h"

#include "Editor/Editor.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR lpCmdLine,
	_In_  int nCmdShow)
{

	std::unique_ptr<MasterEditor> Editor = std::make_unique<MasterEditor>();
	Editor->Start(hInstance);
	Editor->StartBlockingLoop();


	return 0;
}