#include "Default_Headers.h"

#include "Editor/Editor.h"
#include <dxgidebug.h>
#include <dxgi1_3.h>




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

	TAUtils::Paths::instance().SetProjectPath(RootPath + L"\\");

	std::unique_ptr<MasterEditor> Editor = std::make_unique<MasterEditor>(RootPath, hInstance);
	Editor->StartBlockingLoop();

	Editor.reset();
	ImGui_ImplDX11_Shutdown();


	IDXGIDebug* debugDev;
	HRESULT hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugDev));
	hr = debugDev->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

	return 0;
}