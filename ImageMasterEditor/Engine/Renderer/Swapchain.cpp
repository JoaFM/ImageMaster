#include "pch.h"
#include <d3d11.h>

#include "SwapChain.h"
#include "UXUI/Window.h"



SwapChain::SwapChain()
	: Texture()
{
	//SetFriendlyName("SwapChain");
}

SwapChain::~SwapChain()
{
	Release();
	//#TODO need to delete the RT
}

bool SwapChain::CreateSwapChain(
	IM_Math::Int2 size,
	Window* CurWindow,
	Renderer* render,
	ID3D11Device*& out_Device,
	ID3D11DeviceContext*& out_DeviceContext
)
{

	m_Size = size;


	// swap chain
	DXGI_SWAP_CHAIN_DESC swap_chain_descr = { 0 };
	swap_chain_descr.BufferDesc.RefreshRate.Numerator = 0;
	swap_chain_descr.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_descr.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;// DXGI_FORMAT_B8G8R8A8_UNORM;
	swap_chain_descr.SampleDesc.Count = 1;
	swap_chain_descr.SampleDesc.Quality = 0;
	swap_chain_descr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_descr.BufferCount = 2;
	swap_chain_descr.OutputWindow = CurWindow->GetHWND();
	swap_chain_descr.Windowed = true;
	swap_chain_descr.BufferDesc.Width = (UINT)m_Size.x;
	swap_chain_descr.BufferDesc.Height = (UINT)m_Size.y;
	swap_chain_descr.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


	D3D_FEATURE_LEVEL feature_level;
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swap_chain_descr,
		&m_SwapChain,
		&out_Device,
		&feature_level,
		&out_DeviceContext
	);
	assert(S_OK == hr && m_SwapChain && out_Device && out_DeviceContext);
	// Frame buffer

	ID3D11Texture2D* framebuffer;
	hr = m_SwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&framebuffer);
	assert(SUCCEEDED(hr));


	if (!m_SwapRT)
	{
		m_SwapRT = std::make_unique<RenderTarget>("SwapChain default");
	}

	bool Suc = m_SwapRT->CreateTargetFromBuffer(framebuffer, render);

	// 	hr = (*out_Device)->CreateRenderTargetView(
	// 		framebuffer, 0, &m_SwapChainRT);
	// 	assert(SUCCEEDED(hr));
	TA_SAFERELEASE(framebuffer);


	return true;
}

void SwapChain::ResizeToWindow(Renderer* render)
{
	if (!m_SwapRT)
	{
		m_SwapRT = std::make_unique<RenderTarget>("RT for main window");
	}
	else
	{
		m_SwapRT->Release();
	}
	// from - https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi?redirectedfrom=MSDN#handling-window-resizing
	// Release all outstanding references to the swap chain's buffers.
	//TA_SAFERELEASE(m_RenderTargetView);

	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.
	TA_HRCHECK_Simple(m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0),
		L"could not resize the viewport buffer");


	// Get buffer and create a render-target-view.
	ID3D11Texture2D* pBuffer;
	TA_HRCHECK_Simple(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer),
		L"Could not get the swapchains buffer");
	// Perform error handling here!

	bool Suc = m_SwapRT->CreateTargetFromBuffer(pBuffer, render);

	//TA_HRCHECK_Simple(m_Device->CreateRenderTargetView(pBuffer, NULL, &m_RenderTargetView),
	//	L"Could not create new target view for the window");
	//#TODO: Perform error handling here!

	D3D11_TEXTURE2D_DESC desc;
	pBuffer->GetDesc(&desc);

	m_Size = IM_Math::Int2(desc.Width, desc.Height);

	pBuffer->Release();

	//OnNameOpdate();
}

void SwapChain::Present(UINT SyncInterval, UINT Flags)
{
	m_SwapChain->Present(SyncInterval, Flags);
}

// void SwapChain::OnNameOpdate()
// {
// 	TAUtils::SetDebugObjectName(m_SwapChain, GetFriendlyName() + "m_SwapChain");
// 	if (m_SwapRT) m_SwapRT->SetFriendlyName(GetFriendlyName() + "_RT_");
// }

void SwapChain::Release()
{

	TA_SAFERELEASE(m_SwapChain);

}
