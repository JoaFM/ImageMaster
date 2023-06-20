#pragma once

#include "MainApp/Default_Headers.h"
#include "RenderTypes.h"
#include "../IMBase.h"

class Texture : public IMBase
{
public:
	Texture();
	~Texture() { Release(); }

	virtual struct ID3D11UnorderedAccessView* GetUAV() const { return m_UAV; }
	struct ID3D11ShaderResourceView* GetSRV() const { return m_SRV; }
	void SetSampler(RenderTypes::DefaultSamplers NewSampler) { m_MySampler = NewSampler; }
	RenderTypes::DefaultSamplers GetSampler() { return m_MySampler; }

	IM_Math::Int2 GetSize() const { return m_Size; }

protected:
	bool LoadedAndValid = false;
	struct ID3D11ShaderResourceView* m_SRV = nullptr;
	struct ID3D11UnorderedAccessView* m_UAV = nullptr;
	IM_Math::Int2 m_Size;

	struct ID3D11Texture2D* m_textureBuffer = nullptr;
	void Release();

private:
	RenderTypes::DefaultSamplers m_MySampler = RenderTypes::DefaultSamplers::Sampler_WrapPoint;

};

