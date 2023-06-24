#pragma once


#include "pch.h"
#include "Engine/Renderer/Texture.h"


class RenderTarget : public Texture
{
public:
	RenderTarget(std::string FriendlyName);
	~RenderTarget() { Release(); }
	bool CreateTarget(INT32 Width, INT32 height, UINT8 DGIFormat, class Renderer* Render);

	bool CreateTargetFromBuffer(ID3D11Texture2D* framebuffer, class Renderer* Render);
	void Release();

	void Bind(class Renderer* Render);
	void Clear(float R, float G, float B, float A, class Renderer* Render);

	bool SaveLayerToPath(char* Filepath);
	bool CopyBackPixel(IM_Math::Int2 PixelLocation, std::vector<float>& ReadbackBuffer8X8);

	void SetKeepStagingActive(bool ShouldKeepStagingActive) ;
private:
	struct ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	void CreateStencilBuffer(class Renderer* Render);
	void ClearColour(float R, float G, float B, float A, class Renderer* Render);
	void ClearDepth(class Renderer* Render);
	void OnNameOpdate();


	// Staging for readback
	bool CreateStaging();
	void KillStaging();
	ID3D11Texture2D* m_StagingTexture_readBack = nullptr;
	void CopyToStagingTexture();
	void ReadBackDataFromStagingTexture(std::vector<float>& returnBuffer);
private:
	//Depth and stencil
	ID3D11Texture2D* m_DepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	DXGI_FORMAT m_DGIFormat;
	class Renderer* m_Renderer = nullptr;
	bool m_KeepStagingActive = false;


};
