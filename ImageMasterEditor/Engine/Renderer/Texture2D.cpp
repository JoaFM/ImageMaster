#include "pch.h"
#include "Texture2D.h"
#include "Editor/SaveLoad/SaveLoad.h"
#include "Renderer.h"

Texture2D::Texture2D(Renderer* renderer)
{
	m_Renderer = renderer;
}

void Texture2D::LoadReload()
{
	Release();

	unsigned char* image_data = SaveLoad::Load(m_TexturePath.c_str(), &m_Size.x, &m_Size.y, NULL, 4);
	if (image_data == NULL) { return; }


	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_Size.x;
	desc.Height = m_Size.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	m_Renderer->GetDevice()->CreateTexture2D(&desc, &subResource, &m_textureBuffer);


	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_Renderer->GetDevice()->CreateShaderResourceView(m_textureBuffer, &srvDesc, &m_SRV);


	SaveLoad::Load_FreeData(image_data);
}

void Texture2D::SetTexturePath(std::string TexturePath)
{
	m_TexturePath = TexturePath;
}


