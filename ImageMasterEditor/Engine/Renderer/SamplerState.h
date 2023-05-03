#pragma once
#include "MainApp/Default_Headers.h"


class SamplerState
{

public:
	void Bind(UINT SamplerSlot);
	SamplerState(D3D11_SAMPLER_DESC SampleDes, std::string FriendlyName, class  Renderer* renderer);
	~SamplerState();


private:
	struct ID3D11SamplerState* m_SamplerState = nullptr;
	struct ID3D11DeviceContext* m_Device_Context = nullptr;
public:



};
