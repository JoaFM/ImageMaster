#include "Engine/Renderer/Renderer.h"

#include "RenderTarget.h"
#include "Engine/Renderer/Renderer.h"
#include "Editor/SaveLoad/SaveLoad.h"


RenderTarget::RenderTarget(std::string FriendlyName)
	: Texture()
{
	SetFriendlyName("RenderTarget:" + FriendlyName);
}

bool RenderTarget::CreateTarget(INT32 Width, INT32 Height, UINT8 DGIFormat, Renderer* renderer)
{
	m_Renderer = renderer;
	m_DGIFormat = (DXGI_FORMAT)DGIFormat; ;
	Release();
	m_Size = IM_Math::Int2(Width, Height);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (UINT)m_Size.x;
	textureDesc.Height = (UINT)m_Size.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_DGIFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.MiscFlags = 0;


	//#TODO 1st some of the settings may be off 
	//#TODO https://stackoverflow.com/questions/44377201/directx-write-to-texture-with-compute-shader

	TA_HRCHECK(m_Renderer->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_textureBuffer), L"Clould not create renter tartget texture 2D\n");
	TA_HRCHECK(m_Renderer->GetDevice()->CreateRenderTargetView(m_textureBuffer, nullptr, &m_RenderTargetView), L"Could not create a render target view for the texture\n");
	TA_HRCHECK(m_Renderer->GetDevice()->CreateShaderResourceView(m_textureBuffer, nullptr, &m_SRV), L"Could not create a shader resource view for the texture\n");
	
	D3D11_UNORDERED_ACCESS_VIEW_DESC descBuf = {};

	descBuf.Format = DXGI_FORMAT_UNKNOWN;
	descBuf.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D; // or 1d,2d,3d
	descBuf.Buffer.FirstElement = 0;
	descBuf.Buffer.NumElements = m_Size.x * m_Size.y;
	descBuf.Buffer.Flags = D3D11_BIND_UNORDERED_ACCESS; //https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_buffer_uav_flag

	TA_HRCHECK(m_Renderer->GetDevice()->CreateUnorderedAccessView(m_textureBuffer, &descBuf, &m_UAV), L"Failed to make view UAV for buffer");

	CreateStencilBuffer(m_Renderer);
	OnNameOpdate();
	return true;
}

bool RenderTarget::CreateTargetFromBuffer(ID3D11Texture2D* framebuffer, Renderer* Render)
{
	HRESULT hr = Render->GetDevice()->CreateRenderTargetView(
		framebuffer, 0, &m_RenderTargetView);
	assert(SUCCEEDED(hr));

	D3D11_TEXTURE2D_DESC desc;
	framebuffer->GetDesc(&desc);

	m_Size.x = desc.Width;
	m_Size.y = desc.Height;

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
	TA_SAFERELEASE(m_StagingTexture_readBack);
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
	descDepth.Width = (UINT)m_Size.x;
	descDepth.Height = (UINT)m_Size.y;
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

	OnNameOpdate();
}

void RenderTarget::Clear(float R, float G, float B, float A, Renderer* Render)
{
	ClearColour(R, G, B, A, Render);
	ClearDepth(Render);
}


void RenderTarget::OnNameOpdate()
{
	TAUtils::SetDebugObjectName(m_RenderTargetView, GetFriendlyName() + "_RenderTargetView:");
	TAUtils::SetDebugObjectName(m_DepthStencilBuffer, GetFriendlyName() + "_DepthStencilBuffer:");
	TAUtils::SetDebugObjectName(m_DepthStencilView, GetFriendlyName() + "_DepthStencilView:");
	TAUtils::SetDebugObjectName(m_textureBuffer, GetFriendlyName() + "_textureBuffer:");
	TAUtils::SetDebugObjectName(m_SRV, GetFriendlyName() + "_m_SRV:");
}

bool RenderTarget::CreateStaging()
{
	if (m_StagingTexture_readBack) { return true; }

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (UINT)m_Size.x;
	textureDesc.Height = (UINT)m_Size.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_DGIFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_STAGING;// D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = 0;// D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;//D3D11_BIND_RENDER_TARGET |
	textureDesc.MiscFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	TA_HRCHECK(m_Renderer->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_StagingTexture_readBack), L"Clould not create renter tartget texture 2D\n");

	return true;
}

void RenderTarget::KillStaging()
{
	TA_SAFERELEASE(m_StagingTexture_readBack);
}

void RenderTarget::CopyToStagingTexture()
{
	if (m_StagingTexture_readBack != nullptr && m_textureBuffer)
	{
		m_Renderer->GetDeviceContext()->CopyResource(m_StagingTexture_readBack, m_textureBuffer);
	}
}

void RenderTarget::ReadBackDataFromStagingTexture(std::vector<float>& returnBuffer)
{
	if (!CreateStaging()) { return ; }

	CopyToStagingTexture();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_Renderer->GetDeviceContext()->Map(m_StagingTexture_readBack, 0, D3D11_MAP_READ, 0, &mappedResource);
	INT32 TextureSizeInBytes = (GetSize().x * GetSize().y) * 4 * sizeof(float);

	memcpy(&(returnBuffer[0]), mappedResource.pData, TextureSizeInBytes);
	//  Re-enable GPU access to the vertex buffer data.
	m_Renderer->GetDeviceContext()->Unmap(m_StagingTexture_readBack, 0);
}

bool RenderTarget::SaveLayerToPath(char* Filepath)
{
	if (!CreateStaging()) { return false; }

	CopyToStagingTexture();
	
	std::vector<float> cpuData((GetSize().x * GetSize().y) * 4, 0);

	ReadBackDataFromStagingTexture(cpuData);

	if (!m_KeepStagingActive)
	{
		KillStaging();
	}

	SaveLoad::Save(Filepath, m_Size.x, m_Size.y,nullptr, &(cpuData[0]));

	return true;
}

bool RenderTarget::CopyBackPixel(IM_Math::Int2 PixelLocation, std::vector<float>& ReadbackBuffer8X8)
{
	RenderTarget* RT = m_Renderer->GetSmallStageTexture();
	ComputeShader* ComS = m_Renderer->GetComputeShader(L"UTIL_CopyBackDataArea");
	if (!ComS) { return false; }


	MaterialInstance Params;
	Params.m_TexturesOut["BufferOut"] = RT;
	Params.m_TexturesIn["CanvasTexture"] = this;
	Params.SetParamater(std::string("PixelOffset2"), PixelLocation);
	ComS->Dispatch(Params);
	
	RT->ReadBackDataFromStagingTexture(ReadbackBuffer8X8);

	if (!m_KeepStagingActive)
	{
		KillStaging();
	}
	return true;
}

void RenderTarget::SetKeepStagingActive(bool ShouldKeepStagingActive) 
{
	m_KeepStagingActive = ShouldKeepStagingActive;
}
