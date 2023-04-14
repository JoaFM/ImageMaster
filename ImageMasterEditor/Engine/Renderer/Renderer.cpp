


#include "Renderer.h"
#include "UXUI/Window.h"


void Renderer::Init(IM_Math::Int2 size, Window* MainWindow)
{
	//#TODO: Get this data from the window rect
	m_CB_General.RenderBufferSize.x = m_CB_General.DisplayWindowSize.x = (float)size.x;
	m_CB_General.RenderBufferSize.y = m_CB_General.DisplayWindowSize.y = (float)size.y;
	CreateSwapChain(MainWindow);
}

void Renderer::CreateSwapChain(Window* MainWindow)
{

	if (!m_SwapChain)
	{
		m_SwapChain = std::make_unique<SwapChain>();

		m_SwapChain->CreateSwapChain(
			IM_Math::Int2(m_CB_General.DisplayWindowSize.x,m_CB_General.DisplayWindowSize.y),
			MainWindow,
			this,
			m_Device,
			m_Device_Context
		);

		InternalSetRenderTarget(m_SwapChain->GetRenderTarget());
	}


}

void Renderer::Tick()
{
	if (ActiveRenderTarget)
	{
		ActiveRenderTarget->Clear(0.1f, 0.2f, 0.6f, 1.0f, this);
		m_SwapChain->Present(1, 0);
	}
}

void Renderer::InternalSetRenderTarget(RenderTarget* NewRT)
{
	ActiveRenderTarget = NewRT;
	ActiveRenderTarget->Bind(this);
}
