#pragma once

#include "MainApp/Default_Headers.h"
#include "RenderTypes.h"

class Texture
{
public:
	Texture();
	~Texture() { Release(); }


	struct ID3D11ShaderResourceView* GetSRV() { return m_SRV; }
	void SetSampler(RenderTypes::DefaultSamplers NewSampler) { m_MySampler = NewSampler; }
	RenderTypes::DefaultSamplers GetSampler() { return m_MySampler; }

protected:
	bool LoadedAndValid = false;
	struct ID3D11ShaderResourceView* m_SRV = nullptr;
	struct ID3D11Texture2D* m_textureBuffer = nullptr;
	void Release();

private:
	RenderTypes::DefaultSamplers m_MySampler = RenderTypes::DefaultSamplers::Sampler_WrapPoint;

};

