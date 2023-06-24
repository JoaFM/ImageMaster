#pragma once
#include "pch.h"
#include "Engine/Renderer/Texture.h"
#include "RenderTarget.h"

class SwapChain : public Texture
{
public:
	SwapChain();
	~SwapChain();
	bool CreateSwapChain(
		IM_Math::Int2 size,
		class Window* CurWindow,
		class Renderer* render,
		ID3D11Device*& out_Device,
		ID3D11DeviceContext*& out_DeviceContext
	);

	void ResizeToWindow(Renderer* render);

	IM_Math::Int2 GetSize() const { return m_Size; }

	RenderTarget* GetRenderTarget() { return m_SwapRT.get(); }
	void Present(UINT SyncInterval, UINT Flags);

private:
	struct IDXGISwapChain* m_SwapChain = nullptr;
	std::unique_ptr<RenderTarget> m_SwapRT = nullptr;

	IM_Math::Int2 m_Size;
private:
	void Release();

public:
};


