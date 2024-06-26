#include "pch.h"
#include "ConstantBuffer.h"
#include "Utils/IM_STD.h"
#include <d3d11.h>
#include "Renderer.h"

ConstantBuffer::ConstantBuffer(D3D11_BUFFER_DESC constantBufferDesc, std::string FriendlyName, class Renderer* renderer, UINT slot)
{
	m_slot = slot;
	TA_HRCHECK_Simple(
		renderer->GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_CB_Object),
		L"failed to make constant buffer "
	);
	TAUtils::SetDebugObjectName(m_CB_Object, "ConstanBuffer:" + FriendlyName);
	m_Renderer = renderer;
}

ConstantBuffer::~ConstantBuffer()
{
	TA_SAFERELEASE(m_CB_Object);
}

void ConstantBuffer::UpdateData(const void* DataToUpdate)
{
	m_Renderer->GetDeviceContext()->UpdateSubresource(m_CB_Object, 0, nullptr, DataToUpdate, 0, 0);
}



void ConstantBuffer::UnBind_CB()
{
	return;
	if (CSSet)
	{
		m_Renderer->GetDeviceContext()->CSSetConstantBuffers(m_slot, 1, nullptr);
		CSSet = false;
	}

	if (PSSet )
	{
		m_Renderer->GetDeviceContext()->CSSetConstantBuffers(m_slot, 1, nullptr);
		PSSet = false;
	}
	if (VSSet )
	{
		m_Renderer->GetDeviceContext()->CSSetConstantBuffers(m_slot, 1, nullptr);
		VSSet = false;
	}

}

void ConstantBuffer::BindPS(ID3D11PixelShader* PS)
{
	m_Renderer->GetDeviceContext()->PSSetConstantBuffers(m_slot, 1, &m_CB_Object);
	PSSet = true;
}

void ConstantBuffer::BindVS(ID3D11VertexShader* VS) 
{
	m_Renderer->GetDeviceContext()->VSSetConstantBuffers(m_slot, 1, &m_CB_Object);
	CSSet = true;
}
void ConstantBuffer::Bind_CB_to_CS()
{
	m_Renderer->GetDeviceContext()->CSSetConstantBuffers(m_slot, 1, &m_CB_Object);
	CSSet = true;
}