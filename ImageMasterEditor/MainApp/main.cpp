#include "Default_Headers.h"

#include "Editor/Editor.h"




int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR lpCmdLine,
	_In_  int nCmdShow)
{
	// Assumes the exe is 2 / in. Will need to make this adapts for non debug builds 
	WCHAR fileName[MAX_PATH];
	DWORD size = GetModuleFileName(nullptr, fileName, MAX_PATH);
	PathCchRemoveFileSpec(fileName, size);
	PathCchRemoveFileSpec(fileName, size);
	PathCchRemoveFileSpec(fileName, size);

	std::wstring RootPath(fileName);
	std::unique_ptr<MasterEditor> Editor = std::make_unique<MasterEditor>(RootPath, hInstance);
	Editor->StartBlockingLoop();


	return 0;
}