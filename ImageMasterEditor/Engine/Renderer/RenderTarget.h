#pragma once


#include "MainApp/Default_Headers.h"
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

	bool CopyBackData(char* Filepath);


private:
	struct ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	void CreateStencilBuffer(class Renderer* Render);
	void ClearColour(float R, float G, float B, float A, class Renderer* Render);
	void ClearDepth(class Renderer* Render);

	void OnNameOpdate();
private:
	//Depth and stencil
	ID3D11Texture2D* m_DepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	DXGI_FORMAT m_DGIFormat;
	class Renderer* m_Renderer = nullptr;

public:

};
