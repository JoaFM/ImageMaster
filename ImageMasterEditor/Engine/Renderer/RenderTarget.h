#pragma once


#include "MainApp/Default_Headers.h"
#include "Engine/Renderer/Texture.h"


class RenderTarget : public Texture
{
public:
	RenderTarget();
	~RenderTarget() { Release(); }
	bool CreateTarget(INT32 Width, INT32 height, UINT8 DGIFormat, class Renderer* Render);

	bool CreateTargetFromBuffer(ID3D11Texture2D* framebuffer, class Renderer* Render);
	void Release();

	void Bind(class Renderer* Render);
	void Clear(float R, float G, float B, float A, class Renderer* Render);
	struct ID3D11UnorderedAccessView* GetUAV();



	//virtual void OnNameOpdate() override;

private:
	struct ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	void CreateStencilBuffer(class Renderer* Render);
	void ClearColour(float R, float G, float B, float A, class Renderer* Render);
	void ClearDepth(class Renderer* Render);

private:
	IM_Math::Int2 m_Size;
	//Depth and stencil
	ID3D11Texture2D* m_DepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	ID3D11UnorderedAccessView* m_UAV = nullptr;

public:
	IM_Math::Int2 GetSize() const { return m_Size; }

};
