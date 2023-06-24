#include "pch.h"
#include "SamplerState.h"
#include "Renderer.h"


SamplerState::SamplerState(D3D11_SAMPLER_DESC SampleDes, std::string FriendlyName, Renderer* renderer)
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	std::wstring ErrorStr = L"failed to make samples state: Sampler_WrapPoint" + TAUtils::CharToWString(FriendlyName.c_str());
	TA_HRCHECK_Simple(
		renderer->GetDevice()->CreateSamplerState(&samplerDesc, &m_SamplerState),
		ErrorStr
		);
	m_Device_Context = renderer->GetDeviceContext();
}

SamplerState::~SamplerState()
{
	TA_SAFERELEASE(m_SamplerState);
}

void SamplerState::Bind(UINT SamplerSlot)
{
	m_Device_Context->PSSetSamplers(SamplerSlot, 1, &m_SamplerState);
}