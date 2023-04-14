#include "Engine/Renderer/Renderer.h"

#include "RenderTarget.h"
#include "Engine/Renderer/Renderer.h"


RenderTarget::RenderTarget()
	: Texture()
{

	//SetFriendlyName("RenderTarget:");
}

bool RenderTarget::CreateTarget(INT32 Width, INT32 Height, UINT8 DGIFormat, Renderer* Render)
{
	Release();
	m_Width = Width;
	m_Height = Height;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (UINT)m_Width;
	textureDesc.Height = (UINT)m_Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = (DXGI_FORMAT)DGIFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MiscFlags = 0;


	TA_HRCHECK(Render->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_textureBuffer), L"Clould not create renter tartget texture 2D");
	TA_HRCHECK(Render->GetDevice()->CreateRenderTargetView(m_textureBuffer, nullptr, &m_RenderTargetView), L"Could not create a render target view for the texture");
	TA_HRCHECK(Render->GetDevice()->CreateShaderResourceView(m_textureBuffer, nullptr, &m_SRV), L"Could not create a shader resource view for the texture");

	CreateStencilBuffer(Render);
	//OnNameOpdate();
	return true;
}

bool RenderTarget::CreateTargetFromBuffer(ID3D11Texture2D* framebuffer, Renderer* Render)
{
	HRESULT hr = Render->GetDevice()->CreateRenderTargetView(
		framebuffer, 0, &m_RenderTargetView);
	assert(SUCCEEDED(hr));

	D3D11_TEXTURE2D_DESC desc;
	framebuffer->GetDesc(&desc);

	m_Width = desc.Width;
	m_Height = desc.Height;

	CreateStencilBuffer(Render);
	//OnNameOpdate();
	return true;
}

void RenderTarget::Release()
{
	Texture::Release();
	TA_SAFERELEASE(m_RenderTargetView);
	TA_SAFERELEASE(m_DepthStencilBuffer);
	TA_SAFERELEASE(m_DepthStencilView);
}

void RenderTarget::Bind(Renderer* Render)
{
	Render->GetDeviceContext()->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
}


void RenderTarget::ClearColour(float R, float G, float B, float A, Renderer* Render)
{
	const float BGcolor[4] = { R, G, B, A };

	if (m_RenderTargetView)
	{
		Render->GetDeviceContext()->ClearRenderTargetView(m_RenderTargetView, BGcolor);
	}
	else
	{
		//TA_ERROR_C("RT not valid to clear");
	}
}

void RenderTarget::ClearDepth(Renderer* Render)
{
	const float DeaphClear[4] = { 0,0,0,1 };
	if (m_DepthStencilView)
	{
		Render->GetDeviceContext()->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
	}
	else
	{
		//TA_ERROR_C("RT's m_DepthStencilView not valid to clear");
	}
}

void RenderTarget::CreateStencilBuffer(Renderer* Render)
{
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = (UINT)m_Width;
	descDepth.Height = (UINT)m_Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	TA_HRCHECK_Simple(Render->GetDevice()->CreateTexture2D(&descDepth, NULL, &m_DepthStencilBuffer),
		L"Could not create depth and stencil texture");



	//Render->SetStencilState(false);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	TA_HRCHECK_Simple(Render->GetDevice()->CreateDepthStencilView(
		m_DepthStencilBuffer, // Depth stencil texture
		&descDSV, // Depth stencil desc
		&m_DepthStencilView);  // [out] Depth stencil view
	,
		L"Could not create deapth stencil view");

	//OnNameOpdate();
}

void RenderTarget::Clear(float R, float G, float B, float A, Renderer* Render)
{
	ClearColour(R, G, B, A, Render);
	ClearDepth(Render);
}

//void RenderTarget::OnNameOpdate()
//{
	//Texture::OnNameOpdate();
	//TAUtils::SetDebugObjectName(m_RenderTargetView, GetFriendlyName() + "RenderTargetView:");
	//TAUtils::SetDebugObjectName(m_DepthStencilBuffer, GetFriendlyName() + "m_DepthStencilBuffer:");
	//TAUtils::SetDebugObjectName(m_DepthStencilView, GetFriendlyName() + "m_DepthStencilView:");
	//TAUtils::SetDebugObjectName(m_textureBuffer, GetFriendlyName() + "m_textureBuffer:");
	//TAUtils::SetDebugObjectName(m_SRV, GetFriendlyName() + "m_SRV:");
//}

